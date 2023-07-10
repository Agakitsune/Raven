{-# LANGUAGE ForeignFunctionInterface #-}
{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TupleSections #-}
{-# LANGUAGE FlexibleInstances #-}

module Parser where

import Foreign.C.String (CString, peekCAString)
import Foreign.Ptr (nullPtr)
import Text.Megaparsec
import Text.Megaparsec.Char (string, space1, newline)
import Text.Megaparsec.Char.Lexer
import Text.Megaparsec.Debug
import Data.Functor
import Control.Monad
import Control.Arrow
import Data.Text (Text, pack, unpack)
import Data.Void
import Data.Char (isAlpha, isAlphaNum, isDigit, isOctDigit, isHexDigit)
import Data.List
import Data.Tuple (swap)
import Data.Set (Set, singleton, size)
-- import Data.String (fromString)

import GHC.Int (Int64)
import Data.ByteString (useAsCString, length)
import Data.ByteString.Lazy (ByteString, putStr, toStrict)
import qualified Data.ByteString.UTF8 as UTF8
import Data.ByteString.Lazy.UTF8 (fromString)
import Data.Binary.Put
import Data.Word

import System.Environment
import Data.Text.IO
import System.IO (hPutStrLn, stderr)

utf8Len :: Char -> Word8
utf8Len = UTF8.fromString . pure >>> Data.ByteString.length >>> fromIntegral

class RavenSerialize a where
     serialize :: a -> ByteString

type Parser = Parsec Void Text

data Literal = Floating Double
             | Integral Int64
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
                | IndexExpression String Expression
                | ArrayExpression [Expression]
          deriving (Eq)

data Declaration = VariableDeclaration String String (Maybe Expression)
                 | FunctionDeclaration String String [(String, String)] Block
          deriving (Eq)

data Block = Block [Statement]
          deriving (Eq)

data Statement = BlockStatement Block
               | ExpressionStatement Expression
               | LoopStatement Expression Block
               | IfStatement Expression Block (Maybe (Either Block Statement))
               | DeclarationStatement Declaration
               | ReturnStatement (Maybe Expression)
          deriving (Eq)

instance Show Literal where
     show (Floating d) = show d
     show (Integral i) = show i
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
     show (IndexExpression n e) = n ++ "[" ++ show e ++ "]"
     show (ArrayExpression l) = "[" ++ intercalate ", " (map show l) ++ "]"

instance Show Declaration where
     show (VariableDeclaration t n Nothing) = "Variable Declaration " ++ t ++ " " ++ n
     show (VariableDeclaration t n (Just e)) = "Variable Declaration " ++ t ++ " " ++ n ++ " = " ++ show e
     show (FunctionDeclaration t n args blk) = "Function Declaration " ++ t ++ " " ++ n ++ "(" ++ intercalate ", " (map (\(ta,na) -> ta ++ " " ++ na) args) ++ ") " ++ show blk

instance Show Block where
     show (Block lst) = "{\n" ++ intercalate "\n" (map show lst) ++ "\n}"

instance Show Statement where
     show (BlockStatement blk) = show blk
     show (ExpressionStatement e) = show e ++ ";"
     show (LoopStatement e blk) = "While(" ++ show e ++ ") " ++ show blk
     show (IfStatement e blk Nothing) = "If(" ++ show e ++ ") " ++ show blk
     show (IfStatement e blk (Just (Left eblk))) = "If(" ++ show e ++ ") " ++ show blk ++ " else " ++ show eblk
     show (IfStatement e blk (Just (Right i))) = "If(" ++ show e ++ ") " ++ show blk ++ " else " ++ show i
     show (DeclarationStatement s) = show s
     show (ReturnStatement Nothing) = "Return"
     show (ReturnStatement (Just e)) = "Return " ++ show e

instance RavenSerialize String where
     serialize s = fromString s <> fromString "\0"

instance RavenSerialize Literal where
     serialize (Floating d) = runPut $ putWord8 0 <> putWord8 1 <> putDoublebe d
     serialize (Integral i) = runPut $ putWord8 0 <> putWord8 0 <> putInt64be i
     serialize (String s) = runPut (putWord8 1) <> serialize s
     serialize (Boolean True) = runPut $ putWord8 2 <> putWord8 1
     serialize (Boolean False) = runPut $ putWord8 2 <> putWord8 0
     serialize (Character c) = runPut (putWord8 3) <> runPut ((putWord8 . utf8Len) c) <> fromString [c]
     serialize (Null) = runPut $ putWord8 4

instance RavenSerialize Operator where
     serialize (Operator _ _ "~") = runPut $ putWord8 0
     serialize (Operator _ _ "&") = runPut $ putWord8 1
     serialize (Operator _ _ "|") = runPut $ putWord8 2
     serialize (Operator _ _ "^") = runPut $ putWord8 3
     serialize (Operator _ _ "<<") = runPut $ putWord8 4
     serialize (Operator _ _ ">>") = runPut $ putWord8 5
     serialize (Operator _ _ "<<<") = runPut $ putWord8 6
     serialize (Operator _ _ ">>>") = runPut $ putWord8 7
     serialize (Operator _ Unary "-") = runPut $ putWord8 8
     serialize (Operator _ _ "+") = runPut $ putWord8 9
     serialize (Operator _ Binary "-") = runPut $ putWord8 10
     serialize (Operator _ _ "*") = runPut $ putWord8 11
     serialize (Operator _ _ "/") = runPut $ putWord8 12
     serialize (Operator _ _ "%") = runPut $ putWord8 13
     serialize (Operator _ _ "**") = runPut $ putWord8 14
     serialize (Operator _ _ "!") = runPut $ putWord8 15
     serialize (Operator _ _ "==") = runPut $ putWord8 16
     serialize (Operator _ _ "!=") = runPut $ putWord8 17
     serialize (Operator _ _ "<") = runPut $ putWord8 18
     serialize (Operator _ _ ">") = runPut $ putWord8 19
     serialize (Operator _ _ "<=") = runPut $ putWord8 20
     serialize (Operator _ _ ">=") = runPut $ putWord8 21
     serialize (Operator _ _ "&&") = runPut $ putWord8 22
     serialize (Operator _ _ "||") = runPut $ putWord8 23
     serialize (Operator _ _ "^|") = runPut $ putWord8 24
     serialize (PrePostOperator True _ _ "++") = runPut $ putWord8 25
     serialize (PrePostOperator True _ _ "--") = runPut $ putWord8 26
     serialize (PrePostOperator False _ _ "++") = runPut $ putWord8 27
     serialize (PrePostOperator False _ _ "--") = runPut $ putWord8 28
     serialize (Operator _ _ "=") = runPut $ putWord8 29
     serialize (Operator _ _ "+=") = runPut $ putWord8 30
     serialize (Operator _ _ "-=") = runPut $ putWord8 31
     serialize (Operator _ _ "*=") = runPut $ putWord8 32
     serialize (Operator _ _ "/=") = runPut $ putWord8 33
     serialize (Operator _ _ "%=") = runPut $ putWord8 34
     serialize (Operator _ _ "**=") = runPut $ putWord8 35
     serialize (Operator _ _ "&=") = runPut $ putWord8 36
     serialize (Operator _ _ "|=") = runPut $ putWord8 37
     serialize (Operator _ _ "^=") = runPut $ putWord8 38
     serialize (Operator _ _ ">>=") = runPut $ putWord8 39
     serialize (Operator _ _ "<<=") = runPut $ putWord8 40
     serialize (Operator _ _ ">>>=") = runPut $ putWord8 41
     serialize (Operator _ _ "<<<=") = runPut $ putWord8 42

instance RavenSerialize Call where
     serialize (Call n as) = serialize n <> runPut (putWord8 $ fromIntegral $ Data.List.length as) <> foldl (<>) (fromString "") (map serialize as)

instance RavenSerialize Expression where
     serialize (LiteralExpression l) = runPut (putWord8 0) <> serialize l
     serialize (IdentifierExpression i) = runPut (putWord8 1) <> serialize i
     serialize (BinaryExpression e1 o e2) = runPut (putWord8 2) <> serialize o <> serialize e1 <> serialize e2
     serialize (UnaryExpression o e) = runPut (putWord8 2) <> serialize o <> serialize e
     serialize (CallExpression c) = runPut (putWord8 3) <> serialize c
     serialize (FoldedExpression e) = runPut (putWord8 4) <> serialize e
     serialize (IndexExpression n e) = runPut (putWord8 5) <> serialize n <> serialize e
     serialize (ArrayExpression l) = runPut (putWord8 6) <> runPut (putWord8 $ fromIntegral $ Data.List.length l) <> foldl (<>) (fromString "") (map serialize l)

instance RavenSerialize Declaration where
     serialize (VariableDeclaration t n Nothing) = runPut (putWord8 0) <> serialize t <> serialize n <> runPut (putWord8 0)
     serialize (VariableDeclaration t n (Just e)) = runPut (putWord8 0) <> serialize t <> serialize n <> runPut (putWord8 1) <> serialize e
     serialize (FunctionDeclaration t n as blk) = runPut (putWord8 1) <> serialize t <> serialize n <> runPut (putWord8 $ fromIntegral $ Data.List.length as) <> foldl (<>) (fromString "") (map ((uncurry (<>)) . join (***) serialize) as) <> serialize blk

instance RavenSerialize Block where
     serialize (Block lst) = runPut (putWord8 10) <> foldl (<>) (fromString "") (map serialize lst) <> runPut (putWord8 10)

instance RavenSerialize Statement where
     serialize (BlockStatement blk) = runPut (putWord8 0) <> serialize blk
     serialize (ExpressionStatement e) = runPut (putWord8 1) <> serialize e
     serialize (DeclarationStatement dlcr) = runPut (putWord8 2) <> serialize dlcr
     serialize (IfStatement e blk Nothing) = runPut (putWord8 3) <> serialize e <> serialize blk <> runPut (putWord8 0)
     serialize (IfStatement e blk (Just (Right (opt@(IfStatement _ _ _))))) = runPut (putWord8 3) <> serialize e <> serialize blk <> runPut (putWord8 1) <> runPut (putWord8 1) <> serialize opt
     serialize (IfStatement e blk (Just (Left eblk))) = runPut (putWord8 3) <> serialize e <> serialize blk <> runPut (putWord8 1) <> runPut (putWord8 0) <> serialize eblk
     serialize (LoopStatement e blk) = runPut (putWord8 4) <> serialize e <> serialize blk
     serialize (ReturnStatement Nothing) = runPut (putWord8 5) <> runPut (putWord8 0)
     serialize (ReturnStatement (Just e)) = runPut (putWord8 5) <> runPut (putWord8 1) <> serialize e

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

floatLiteral :: Parser Double
floatLiteral = do
     o <- getOffset
     r <- observing $ (:) <$> (num1_9 <|> single '0') <*> many num >>= \i -> (single '.' *> some num <* (notFollowedBy (single 'e' <|> single 'E' <|> single '.')) >>= \f -> pure $ read (i ++ "." ++ f))
     case r of
          (Left _) -> setOffset o *> fail "invalid number literal"
          (Right f) -> pure f

integralLiteral :: Parser Int64
integralLiteral = do
     o <- getOffset
     r <- observing $ string "0x" *> hexadecimal
          <|> string "0o" *> octal
          <|> string "0b" *> binary
          <|> (decimal <* (notFollowedBy (single '.')))
     case r of
          (Left _) -> setOffset o *> fail "invalid number literal"
          (Right n) -> pure n

boolean :: Parser Bool
boolean = (string "true" $> True) <|> (string "false" $> False) <|> fail "invalid boolean literal"

nullLiteral :: Parser Literal
nullLiteral = string "null" $> Null

literal :: Parser Literal
literal = do
     o <- getOffset
     r <- observing $ (try $ Floating <$> floatLiteral) <|> (Integral <$> integralLiteral) <|> (Boolean <$> boolean) <|> (String <$> stringLiteral) <|> (Character <$> character)
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
          then arrayType r
          else setOffset o *> fail "expected type"

arrayType :: String -> Parser String
arrayType str = do
     r <- observing $ whitespace *> single '['
     case r of
          (Left _) -> pure str
          (Right _) -> do
               o <- getOffset
               r <- observing $ whitespace *> single ']'
               case r of
                    (Left _) -> setOffset o *> fail "expected ']'"
                    (Right _) -> arrayType $ str ++ "[]"

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
numberLiteralExpression = LiteralExpression <$> ((Floating <$> floatLiteral) <|> (Integral <$> integralLiteral))

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

indexExpression :: Parser Expression
indexExpression = do
     id <- observing $ identifier <* whitespace
     case id of
          (Left _) -> fail "expected identifier"
          (Right id) -> do
               r <- observing $ single '[' <* whitespace
               case r of
                    (Left _) -> fail "expected '['"
                    (Right _) -> do
                         exp <- observing $ expression <* whitespace
                         case exp of
                              (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                              (Right idx) -> do
                                   r <- observing $ single ']'
                                   case r of
                                        (Left _) -> fail "expected ']'"
                                        (Right _) -> pure $ IndexExpression id idx

arrayExpression :: Parser Expression
arrayExpression = do
     r <- observing $ single '[' <* whitespace
     case r of
          (Left _) -> fail "expected '['"
          (Right _) -> do
               values <- observing $ sepBy expression (single ',') <* whitespace
               case values of
                    (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                    (Right values) -> do
                         r <- observing $ single ']'
                         case r of
                              (Left _) -> fail "expected ']'"
                              (Right _) -> pure $ ArrayExpression values

-- leafExpression :: Parser Expression
-- leafExpression = try (dbg "leaf fold" $ foldExpression) <|> try (dbg "leaf call" $ callExpression) <|> (dbg "leaf literal" $ LiteralExpression <$> literal) <|> (dbg "leaf id" $ identifierExpression)

leafExpression :: Parser Expression
leafExpression = try foldExpression <|> try callExpression <|> try indexExpression <|> try arrayExpression <|> (LiteralExpression <$> literal) <|> (identifierExpression <* whitespace <* notFollowedBy (single '('))

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
                    (Left _) -> do
                         if size s == 1
                         then setOffset i *> fancyFailure s
                         else setOffset i *> fail "invalid expression"
          (Right e) -> pure e


block :: Parser Block
block = do
     r <- observing $ single '{' <* whitespace
     case r of
          (Left _) -> fail "expected '{'"
          (Right _) -> do
               blk <- observing (manyTill inlineBlockParser (single '}'))
               case blk of
                    (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                    (Left (TrivialError i s u)) -> setOffset i *> failure s u
                    (Right blk) -> do pure $ Block blk

expressionStatement :: Parser Statement
expressionStatement = ExpressionStatement <$> (expression <* whitespace <* termination)

blockStatement :: Parser Statement
blockStatement = BlockStatement <$> block

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
                                        (Left (TrivialError i s u)) -> do
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
loopStatement = LoopStatement <$> (keyword "while" *> inlineExpression <* whitespace) <*> block

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
                         blk <- observing $ block
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
                                                            blkee <- observing $ block
                                                            case blkee of
                                                                 (Right blkee) -> pure $ IfStatement expr blk (Just (Left blkee))
                                                                 (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                                                                 (Left (TrivialError i s u)) -> setOffset i *> failure s u
                                                  (Right ifee) -> pure $ IfStatement expr blk (Just (Right ifee))

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
                         then do (pure $ ReturnStatement Nothing) <* termination
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
     btype <- observing $ bundledType <* whitespace
     case btype of
          (Left (TrivialError i s u)) -> setOffset i *> failure s u
          (Left (FancyError i s)) -> setOffset i *> fancyFailure s
          (Right btype) -> do
               name <- observing identifier
               case name of
                    (Left _) -> fail "expected identifier"
                    (Right name) -> VariableDeclaration btype name <$> optional variableDeclarator

variableDeclarationStatement :: Parser Statement
variableDeclarationStatement = DeclarationStatement <$> (variableDeclaration <* whitespace <* termination)

functionArg :: Parser (String, String)
functionArg = do
     btype <- observing $ bundledType <* whitespace
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
                                                       blk <- observing $ block
                                                       case blk of
                                                            (Left (FancyError i s)) -> setOffset i *> fancyFailure s
                                                            (Left (TrivialError i s u)) -> setOffset i *> failure s u
                                                            (Right blk) -> pure $ FunctionDeclaration btype id args blk
     -- FunctionDeclaration <$> bundledType <*> identifier <* whitespace <*> (between (single '(') (single ')')) (whitespace *> sepBy ((,) <$> bundledType <*> identifier) (single ',') <* whitespace) <* whitespace <*> blockStatement

ravenParser :: Parser [Declaration]
ravenParser = many (whitespace *> (try functionDeclaration <|> (variableDeclaration <* whitespace <* termination)) <* whitespace) <* eof

serializeTest :: RavenSerialize a => a -> IO ()
serializeTest a = Data.ByteString.Lazy.putStr $ serialize a
