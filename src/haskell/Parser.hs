{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TupleSections #-}

module Parser where

import Foreign.C.Types
import Text.Megaparsec
import Text.Megaparsec.Char (string, space1, newline)
import Text.Megaparsec.Char.Lexer
import Text.Megaparsec.Debug
import Data.Functor
import Control.Monad
import Data.Text (Text, pack, unpack)
import Data.Void
import Data.Char (isAlpha, isAlphaNum, isDigit, isOctDigit, isHexDigit)
import Data.List
import Data.Tuple (swap)
import Data.Set (Set, singleton)

import System.Environment
import Data.Text.IO

type Parser = Parsec Void Text

data Literal = Number Double
             | String String
             | Boolean Bool
             | Character Char
             | Null
          deriving (Eq)

data OperatorType = Bitwise | Arithmetic | Logical | Comparison | Update | Assignment
          deriving (Eq)

data OperatorDimension = Unary | Binary
          deriving (Eq)

data Operator = Operator { typee :: OperatorType, dimension :: OperatorDimension, value :: String }
              | PrePostOperator { post :: Bool, typee :: OperatorType, dimension :: OperatorDimension, value :: String }
          deriving (Eq)

data Call = Call { name :: String, arguments :: [Expression] }
          deriving (Eq)

data Expression = BinaryExpression Expression Operator Expression
                | UnaryExpression Operator Expression
                | CallExpression Call
                | IdentifierExpression String
                | LiteralExpression Literal
                | FoldedExpression Expression
          deriving (Eq)

data Declaration = VariableDeclaration String String (Maybe Expression)
                 | FunctionDeclaration String String [(String, String)] Statement
          deriving (Eq)

data Statement = BlockStatement [Statement]
               | ExpressionStatement Expression
               | LoopStatement Expression Statement
               | IfStatement Expression Statement (Maybe Statement)
               | DeclarationStatement Declaration
               | ReturnStatement (Maybe Expression)
          deriving (Eq)

instance Show Literal where
     show (Number d) = show d
     show (String s) = show s
     show (Boolean b) = show b
     show (Character c) = show c
     show Null = "null"

instance Show Operator where
     show (Operator Bitwise Unary s) = "Bitwise Unary " ++ s
     show (Operator Bitwise Binary s) = "Bitwise Binary " ++ s
     show (Operator Arithmetic Unary s) = "Arithmetic Unary " ++ s
     show (Operator Arithmetic Binary s) = "Arithmetic Binary " ++ s
     show (Operator Logical Unary s) = "Logical Unary " ++ s
     show (Operator Logical Binary s) = "Logical Binary " ++ s
     show (Operator Comparison Unary s) = "Comparison Unary " ++ s
     show (Operator Comparison Binary s) = "Comparison Binary " ++ s
     show (PrePostOperator True Update Unary s) = "Update Unary (postfix) " ++ s
     show (PrePostOperator False Update Unary s) = "Update Unary (prefix) " ++ s
     show (Operator Assignment Binary s) = "Assignment Binary " ++ s

instance Show Call where
     show (Call n a) = "Call " ++ n ++ " " ++ show a

instance Show Expression where
     show (BinaryExpression l o r) = "BinaryExpression " ++ show l ++ " " ++ show o ++ " " ++ show r
     show (UnaryExpression o l) = "UnaryExpression " ++ show o ++ " " ++ show l
     show (CallExpression c) = "CallExpression " ++ show c
     show (IdentifierExpression i) = "IdentifierExpression " ++ i
     show (LiteralExpression l) = "LiteralExpression " ++ show l
     show (FoldedExpression e) = "(" ++ show e ++ ")"

instance Show Declaration where
     show (VariableDeclaration t n Nothing) = "Variable Declaration " ++ t ++ " " ++ n
     show (VariableDeclaration t n (Just e)) = "Variable Declaration " ++ t ++ " " ++ n ++ " = " ++ show e
     show (FunctionDeclaration t n args blk) = "Function Declaration " ++ t ++ " " ++ n ++ "(" ++ intercalate ", " (map (\(ta,na) -> ta ++ " " ++ na) args) ++ ") " ++ show blk

instance Show Statement where
     show (BlockStatement lst) = "{\n" ++ intercalate "\n" (map show lst) ++ "\n}"
     show (ExpressionStatement e) = show e ++ ";"
     show (LoopStatement e blk) = "While(" ++ show e ++ ") " ++ show blk
     show (IfStatement e blk Nothing) = "If(" ++ show e ++ ") " ++ show blk
     show (IfStatement e blk (Just s)) = "If(" ++ show e ++ ") " ++ show blk ++ " else " ++ show s
     show (DeclarationStatement s) = show s
     show (ReturnStatement Nothing) = "Return"
     show (ReturnStatement (Just e)) = "Return " ++ show e

alpha :: Parser Char
alpha = satisfy isAlpha

alphaNum :: Parser Char
alphaNum = satisfy isAlphaNum

num :: Parser Char
num = satisfy isDigit

num1_9 :: Parser Char
num1_9 = satisfy (\c -> c >= '1' && c <= '9')

character :: Parser Char
character = do
     o <- getOffset
     r <- observing $ between (single '\'') (single '\'') charLiteral
     case r of
          (Left _) -> setOffset o *> fail "invalid character literal"
          (Right c) -> pure c

stringLiteral :: Parser String
stringLiteral = do
     o <- getOffset
     r <- observing $ (single '\"') *> manyTill charLiteral (single '\"')
     case r of
          (Left _) -> setOffset o *> fail "invalid string literal"
          (Right str) -> pure str

number :: Parser Double
number = do
     o <- getOffset
     r <- observing $ string "0x" *> hexadecimal
          <|> string "0o" *> octal
          <|> string "0b" *> binary
          <|> try floatLiteral <|> (decimal <* (notFollowedBy (single '.')))
     case r of
          (Left _) -> setOffset o *> fail "invalid number literal"
          (Right n) -> pure n

floatLiteral :: Parser Double
floatLiteral = (:) <$> (num1_9 <|> single '0') <*> many num >>= \i -> (single '.' *> many num <* (notFollowedBy (single 'e' <|> single 'E' <|> single '.')) >>= \f -> pure $ read (i ++ "." ++ f)) <|> pure (read i)

boolean :: Parser Bool
boolean = (string "true" $> True) <|> (string "false" $> False) <|> fail "invalid boolean literal"

nullLiteral :: Parser Literal
nullLiteral = string "null" $> Null

literal :: Parser Literal
literal = do
     o <- getOffset
     r <- observing $ (Number <$> number) <|> (Boolean <$> boolean) <|> (String <$> stringLiteral) <|> (Character <$> character)
     case r of
          (Left _) -> setOffset o *> fail "invalid literal"
          (Right l) -> pure l

keywordList = ["true", "false", "if", "else", "while", "return", "null"]
bundledTypeList = ["Character", "Short", "Integer", "Long", "Float", "Double", "String", "Boolean"]

keyword :: String -> Parser String
keyword s = do
     o <- getOffset
     r <- observing $ whitespace *> (unpack <$> string ( pack s)) <* whitespace
     case r of
          (Left _) -> setOffset o *> fail ("expected '" ++ s ++ "' keyword")
          (Right k) -> pure k

bundledType :: Parser String
bundledType = do
     o <- getOffset
     r <- whitespace *> identifier' <* whitespace
     if r `elem` bundledTypeList
          then pure r
          else setOffset o *> fail "expected type"

notKeyword :: Parser String -> Parser String
notKeyword p = do
     o <- getOffset
     r <- p
     if r `elem` keywordList
          then setOffset o *> fail "unexpected keyword"
          else pure r

notType :: Parser String -> Parser String
notType p = do
     o <- getOffset
     r <- p
     if r `elem` bundledTypeList
          then setOffset o *> fail "unexpected type"
          else pure r

identifier' :: Parser String
identifier' = (:) <$> (alpha <|> single '_' ) <*> many (alphaNum <|> single '_')

identifier :: Parser String
identifier = do
     o <- getOffset
     r <- observing $ notType $ notKeyword $ identifier'
     case r of
          (Left _) -> setOffset o *> fail "invalid identifier"
          (Right i) -> pure i

termination :: Parser ()
termination = void ((single ';') <|> (fail "expected ';'"))

bitwiseUnaryOperator :: Parser Operator
bitwiseUnaryOperator = string "~" $> Operator Bitwise Unary "~"

bitwiseBinaryOperator :: Parser Operator
bitwiseBinaryOperator = (string "&" $> Operator Bitwise Binary "&")
                    <|> (string "|" $> Operator Bitwise Binary "|")
                    <|> (string "^" $> Operator Bitwise Binary "^")
                    <|> (string ">>>" $> Operator Bitwise Binary ">>>")
                    <|> (string "<<<" $> Operator Bitwise Binary "<<<")
                    <|> (string "<<" $> Operator Bitwise Binary "<<")
                    <|> (string ">>" $> Operator Bitwise Binary ">>")

arithmeticUnaryOperator :: Parser Operator
arithmeticUnaryOperator = (string "-" $> Operator Arithmetic Unary "-")
                      <|> (string "+" $> Operator Arithmetic Unary "+")

arithmeticBinaryOperator :: Parser Operator
arithmeticBinaryOperator = (string "+" $> Operator Arithmetic Binary "+")
                       <|> (string "-" $> Operator Arithmetic Binary "-")
                       <|> (string "**" $> Operator Arithmetic Binary "**")
                       <|> (string "*" $> Operator Arithmetic Binary "*")
                       <|> (string "/" $> Operator Arithmetic Binary "/")
                       <|> (string "%" $> Operator Arithmetic Binary "%")

logicalUnaryOperator :: Parser Operator
logicalUnaryOperator = string "!" $> Operator Logical Unary "!"

logicalBinaryOperator :: Parser Operator
logicalBinaryOperator = (string "&&" $> Operator Logical Binary "&&")
                    <|> (string "||" $> Operator Logical Binary "||")
                    <|> (string "^|" $> Operator Logical Binary "^|")

comparisonBinaryOperator :: Parser Operator
comparisonBinaryOperator = (string "==" $> Operator Comparison Binary "==")
                       <|> (string "!=" $> Operator Comparison Binary "!=")
                       <|> (string "<=" $> Operator Comparison Binary "<=")
                       <|> (string ">=" $> Operator Comparison Binary ">=")
                       <|> (string "<" $> Operator Comparison Binary "<")
                       <|> (string ">" $> Operator Comparison Binary ">")

assignmentBinaryOperator :: Parser Operator
assignmentBinaryOperator = (string "+=" $> Operator Assignment Binary "+=")
                       <|> (string "-=" $> Operator Assignment Binary "-=")
                       <|> (string "*=" $> Operator Assignment Binary "*=")
                       <|> (string "/=" $> Operator Assignment Binary "/=")
                       <|> (string "%=" $> Operator Assignment Binary "%=")
                       <|> (string "**=" $> Operator Assignment Binary "**=")
                       <|> (string "&=" $> Operator Assignment Binary "&=")
                       <|> (string "|=" $> Operator Assignment Binary "|=")
                       <|> (string "^=" $> Operator Assignment Binary "^=")
                       <|> (string ">>=" $> Operator Assignment Binary ">>=")
                       <|> (string "<<=" $> Operator Assignment Binary "<<=")
                       <|> (string ">>>=" $> Operator Assignment Binary ">>>=")
                       <|> (string "<<<=" $> Operator Assignment Binary "<<<=")

updatePreUnaryOperator :: Parser Operator
updatePreUnaryOperator = (string "++" $> PrePostOperator False Update Unary "++")
                     <|> (string "--" $> PrePostOperator False Update Unary "--")

updatePostUnaryOperator :: Parser Operator
updatePostUnaryOperator = (string "++" $> PrePostOperator True Update Unary "++")
                      <|> (string "--" $> PrePostOperator True Update Unary "--")

unaryOperator :: Parser Operator
unaryOperator = do
     o <- getOffset
     r <- observing $ bitwiseUnaryOperator
            <|> arithmeticUnaryOperator
            <|> logicalUnaryOperator
     case r of
          (Left _) -> setOffset o *> fail "invalid unary operator"
          (Right o) -> pure o

binaryOperator :: Parser Operator
binaryOperator = do
     o <- getOffset
     r <- observing $ assignmentBinaryOperator
             <|> logicalBinaryOperator
             <|> bitwiseBinaryOperator
             <|> arithmeticBinaryOperator
             <|> comparisonBinaryOperator
             <|> (string "=" $> Operator Assignment Binary "=") -- assignment = operator is last because comparison operators have ==
     case r of
          (Left _) -> setOffset o *> fail "invalid binary operator"
          (Right o) -> pure o

whitespace :: Parser ()
whitespace = label "whitespace" $ void (many (space1 <|> (void newline)))

inlineExpression :: Parser Expression
inlineExpression = do
     r <- observing $ single '(' <* whitespace
     case r of
          (Left _) -> fail "expected '('"
          (Right _) -> do
               fold <- observing expression
               case fold of
                    (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                    (Right exp) -> do
                         r <- observing $ whitespace *> single ')'
                         case r of
                              (Left _) -> fail "expected ')'"
                              (Right _) -> pure exp

foldExpression :: Parser Expression
foldExpression = FoldedExpression <$> inlineExpression

identifierExpression :: Parser Expression
identifierExpression = IdentifierExpression <$> identifier

numberLiteralExpression :: Parser Expression
numberLiteralExpression = LiteralExpression <$> Number <$> number

callExpression :: Parser Expression
callExpression = do
     id <- observing $ identifier <* whitespace
     case id of
          (Left _) -> fail "expected identifier"
          (Right id) -> do
               r <- observing $ single '(' <* whitespace
               case r of
                    (Left _) -> fail "expected '('"
                    (Right _) -> do
                         args <- observing $ sepBy expression (single ',') <* whitespace
                         case args of
                              (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                              (Right args) -> do
                                   r <- observing $ single ')'
                                   case r of
                                        (Left _) -> fail "expected ')'"
                                        (Right _) -> pure $ CallExpression $ Call id args

-- leafExpression :: Parser Expression
-- leafExpression = try (dbg "leaf fold" $ foldExpression) <|> try (dbg "leaf call" $ callExpression) <|> (dbg "leaf literal" $ LiteralExpression <$> literal) <|> (dbg "leaf id" $ identifierExpression)

leafExpression :: Parser Expression
leafExpression = try foldExpression <|> try callExpression <|> (LiteralExpression <$> literal) <|> (identifierExpression <* whitespace <* notFollowedBy (single '('))

unaryExpression :: Parser Expression
unaryExpression = UnaryExpression <$> unaryOperator <* whitespace <*> leafExpression

preUpdateExpression :: Parser Expression
preUpdateExpression = UnaryExpression <$> updatePreUnaryOperator <*  whitespace <*> leafExpression

-- postUpdateExpression :: Parser Expression
-- postUpdateExpression = dbg "postupdate" $ uncurry UnaryExpression <$> ((<$>) swap $ (,) <$> (dbg "post leaf" $ leafExpression) <* whitespace <*> updatePostUnaryOperator)

postUpdateExpression :: Parser Expression
postUpdateExpression = uncurry UnaryExpression <$> ((<$>) swap $ (,) <$> leafExpression <* whitespace <*> updatePostUnaryOperator)

-- mutateExpression :: Expression -> Parser Expression
-- mutateExpression e = dbg "mutate" $ (try $ BinaryExpression e <$> (dbg "mutate operator" $ binaryOperator) <* whitespace <*> (dbg "mutate expression" $ expression)) <|> (dbg "original expression" $ pure e)

-- mutateExpression :: Expression -> Parser Expression
-- mutateExpression e = (try $ BinaryExpression e <$> (whitespace *> binaryOperator) <* whitespace <*> expression) <|> (try $ uncurry UnaryExpression <$> ((,e) <$> (whitespace *> updatePostUnaryOperator))) <|> pure e

mutateExpression :: Expression -> Parser Expression
mutateExpression e = do
     o <- getOffset
     i <- getInput
     op <- observing $ (whitespace *> binaryOperator)
     case op of
          (Left _) -> setInput i *> (try $ uncurry UnaryExpression <$> ((,e) <$> (whitespace *> updatePostUnaryOperator))) <|> pure e
          (Right (Operator Assignment dim "=")) -> do
               exp <- observing $ (whitespace *> (try expression <|> (LiteralExpression <$> nullLiteral)))
               case exp of
                    (Left _) -> setOffset o *> setInput i *> (try $ uncurry UnaryExpression <$> ((,e) <$> (whitespace *> updatePostUnaryOperator))) <|> fail "invalid expression"
                    (Right exp) -> pure $ BinaryExpression e (Operator Assignment dim "=") exp
          (Right op) -> do
               exp <- observing $ (whitespace *> expression)
               case exp of
                    (Left (FancyError off s)) -> setOffset o *> setInput i *> (uncurry UnaryExpression <$> try ((,e) <$> (whitespace *> updatePostUnaryOperator)) <|> (setOffset off *> fancyFailure s))
                    (Right exp) -> pure $ BinaryExpression e op exp

expression :: Parser Expression
expression = do
     end <- whitespace *> atEnd
     if end
     then do fail "expected expression"
     else do
     r <- observing $ mutateExpression =<< try preUpdateExpression <|> unaryExpression <|> leafExpression
     case r of
          (Left (TrivialError i s u)) -> setOffset i *> fail "invalid expression"
          (Left (FancyError i s)) -> do
               term <- observing $ (whitespace *> (lookAhead $ single ';'))
               case term of
                    (Right _) -> fail "expected expression"
                    (Left _) -> setOffset i *> fancyFailure s
          (Right e) -> pure e


expressionStatement :: Parser Statement
expressionStatement = ExpressionStatement <$> (expression <* whitespace <* termination)

blockStatement :: Parser Statement
blockStatement = do
     r <- observing $ single '{' <* whitespace
     case r of
          (Left _) -> fail "expected '{'"
          (Right _) -> do
               blk <- observing (manyTill inlineBlockParser (single '}'))
               case blk of
                    (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                    (Left (TrivialError i s u)) -> setOffset i *> failure s u
                    (Right blk) -> do pure $ BlockStatement blk

inlineBlockParser :: Parser Statement
inlineBlockParser = do
     end <- whitespace *> atEnd
     if end
     then do fail "expected '}'"
     else do
          exp <- observing $ statement' True <* whitespace
          case exp of
               (Left (FancyError i s)) -> setOffset i *> fancyFailure s
               (Left (TrivialError i s u)) -> setOffset i *> failure s u
               (Right exp) -> pure exp

statement' :: Bool -> Parser Statement
statement' inBlk = do
     input <- getInput
     o <- getOffset
     loop <- observing loopStatement
     case loop of
          (Right loop) -> do
               r <- observing $ whitespace *> (lookAhead $ single '}')
               case r of
                    (Left _) -> pure loop
                    (Right _) -> do
                    if inBlk
                    then do pure loop
                    else do setOffset o *> fail "expected '{'"
          (Left (TrivialError i s u)) -> setOffset i *> failure s u
          (Left (FancyError i s)) -> do
               if (s /= (singleton (ErrorFail "expected 'while' keyword")))
               then do setOffset i *> fancyFailure s
               else do
               condi <- setInput input *> observing ifStatement
               case condi of
                    (Right condi) -> do
                         r <- observing $ whitespace *> (lookAhead $ single '}')
                         case r of
                              (Left _) -> pure condi
                              (Right _) -> do
                              if inBlk
                              then do pure condi
                              else do setOffset o *> fail "expected '{'"
                    (Left (FancyError i s)) -> do
                         if (s /= (singleton (ErrorFail "expected 'if' keyword")))
                         then do setOffset i *> fancyFailure s
                         else do
                         blk <- setInput input *> observing blockStatement
                         case blk of
                              (Right blk) -> do
                                   r <- observing $ whitespace *> (lookAhead $ single '}')
                                   case r of
                                        (Left _) -> pure blk
                                        (Right _) -> do
                                        if inBlk
                                        then do pure blk
                                        else do setOffset o *> fail "expected '{'"
                              (Left (FancyError i s)) -> do
                                   if (s /= (singleton (ErrorFail "expected '{'")))
                                   then do setOffset i *> fancyFailure s
                                   else do
                                   variable <- setInput input *> observing variableDeclarationStatement
                                   case variable of
                                        (Right variable) -> do
                                             r <- observing $ whitespace *> (lookAhead $ single '}')
                                             case r of
                                                  (Left _) -> pure variable
                                                  (Right _) -> do
                                                  if inBlk
                                                  then do pure variable
                                                  else do setOffset o *> fail "expected '{'"
                                        (Left (FancyError i s)) -> do
                                             if (s /= (singleton (ErrorFail "expected type")))
                                             then do setOffset i *> fancyFailure s
                                             else do
                                             ret <- setInput input *> observing returnStatement
                                             case ret of
                                                  (Right ret) -> do
                                                       r <- observing $ whitespace *> (lookAhead $ single '}')
                                                       case r of
                                                            (Left _) -> pure ret
                                                            (Right _) -> do
                                                            if inBlk
                                                            then do pure ret
                                                            else do setOffset o *> fail "expected '{'"
                                                  (Left (FancyError i s)) -> do
                                                       if (s /= (singleton (ErrorFail "expected 'return' keyword")))
                                                       then do setOffset i *> fancyFailure s
                                                       else do
                                                       exp <- setInput input *> observing expressionStatement
                                                       case exp of
                                                            (Right exp) -> do
                                                                 r <- observing $ whitespace *> (lookAhead $ single '}')
                                                                 case r of
                                                                      (Left _) -> pure exp
                                                                      (Right _) -> do
                                                                      if inBlk
                                                                      then do pure exp
                                                                      else do setOffset o *> fail "expected '{'"
                                                            (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                                                            (Left (TrivialError i s u)) -> setOffset i *> failure s u

statement :: Parser Statement
statement = statement' False

loopStatement :: Parser Statement
loopStatement = LoopStatement <$> (keyword "while" *> inlineExpression <* whitespace) <*> blockStatement

ifStatement :: Parser Statement
ifStatement = do
     key <- observing $ keyword "if"
     case key of
          (Left (FancyError i s)) -> setOffset i *> fancyFailure s
          (Right _) -> do
               expr <- observing $ inlineExpression <* whitespace
               case expr of
                    (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                    (Right expr) -> do
                         blk <- observing $ blockStatement
                         case blk of
                              (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                              (Right blk) -> do
                                   elsee <- observing $ whitespace *> keyword "else"
                                   case elsee of
                                        (Left _) -> pure $ IfStatement expr blk Nothing
                                        (Right _) -> do
                                             ifee <- observing $ whitespace *> ifStatement
                                             case ifee of
                                                  (Left (FancyError i s)) -> do
                                                       if (s /= (singleton (ErrorFail "expected 'if' keyword")))
                                                       then do setOffset i *> fancyFailure s
                                                       else do
                                                            blkee <- observing $ blockStatement
                                                            case blkee of
                                                                 (Right blkee) -> pure $ IfStatement expr blk (Just blkee)
                                                                 (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                                                                 (Left (TrivialError i s u)) -> setOffset i *> failure s u
                                                  (Right ifee) -> pure $ IfStatement expr blk (Just ifee)

returnStatement :: Parser Statement
returnStatement = do
     key <- observing $ keyword "return" <* whitespace
     case key of
          (Left (FancyError i s)) -> setOffset i *> fancyFailure s
          (Right _) -> do
               off <- getOffset
               expr <- observing $ expression
               case expr of
                    (Left (FancyError i s)) -> do
                         if (i == off)
                         then do pure $ ReturnStatement Nothing
                         else do setOffset i *> fancyFailure s
                    (Right expr) -> do
                         term <- observing $ whitespace *> termination
                         case term of
                              (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                              (Right _) -> pure $ ReturnStatement (Just expr)

variableDeclarator :: Parser Expression
variableDeclarator = do
     r <- observing $ (whitespace *> single '=' <* whitespace)
     case r of
          (Left _) -> fail "expected '='"
          (Right _) -> do
               input <- getInput
               off <- getOffset
               exp <- observing $ expression
               case exp of
                    (Left (FancyError i s)) -> do
                         null <- setOffset off *> setInput input *> (observing $ (LiteralExpression <$> nullLiteral))
                         case null of
                              (Left _) -> setOffset i *> fancyFailure s
                              (Right null) -> pure null
                    (Right exp) -> pure exp

variableDeclaration :: Parser Declaration
variableDeclaration = do
     btype <- observing bundledType
     case btype of
          (Left _) -> fail "expected type"
          (Right btype) -> do
               name <- observing identifier
               case name of
                    (Left _) -> fail "expected identifier"
                    (Right name) -> VariableDeclaration btype name <$> optional variableDeclarator

variableDeclarationStatement :: Parser Statement
variableDeclarationStatement = DeclarationStatement <$> (variableDeclaration <* whitespace <* termination)

functionArg :: Parser (String, String)
functionArg = do
     btype <- observing bundledType
     case btype of
          (Left _) -> fail "expected type"
          (Right btype) -> do
               id <- observing identifier
               case id of
                    (Left _) -> fail "expected identifier"
                    (Right id) -> pure (btype, id)

functionDeclaration :: Parser Declaration
functionDeclaration = do
     btype <- observing bundledType
     case btype of
          (Left (TrivialError i s u)) -> setOffset i *> failure s u
          (Left (FancyError i s)) -> setOffset i *> fancyFailure s
          (Right btype) -> do
               id <- observing $ identifier <* whitespace
               case id of
                    (Left _) -> fail "expected identifier"
                    (Right id) -> do
                         r <- observing $ single '(' <* whitespace
                         case r of
                              (Left _) -> fail "expected '('"
                              (Right _) -> do
                                   args <- observing $ sepBy functionArg (single ',') <* whitespace
                                   case args of
                                        (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                                        (Right args) -> do
                                             r <- observing $ single ')' <* whitespace
                                             case r of
                                                  (Left _) -> fail "expected ')'"
                                                  (Right _) -> do
                                                       blk <- observing $ blockStatement
                                                       case blk of
                                                            (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                                                            (Right blk) -> pure $ FunctionDeclaration btype id args blk
     -- FunctionDeclaration <$> bundledType <*> identifier <* whitespace <*> (between (single '(') (single ')')) (whitespace *> sepBy ((,) <$> bundledType <*> identifier) (single ',') <* whitespace) <* whitespace <*> blockStatement

ravenParser :: Parser [Declaration]
ravenParser = many (whitespace *> (functionDeclaration <|> (variableDeclaration <* whitespace <* termination)) <* whitespace) <* eof
