{-# LANGUAGE OverloadedStrings #-}

module Main where

import Parser
import Data.Text
import Data.Void
import Test.Hspec
import Test.Hspec.Megaparsec
import Text.Megaparsec
import Text.Megaparsec.Error
import Text.Megaparsec.Char

characterTest :: SpecWith (Arg Expectation)
characterTest = do
    it "character" $
        parse character "" "'a'" `shouldParse` 'a'
    it "character Failure" $
        parse character "" "a" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid character literal")
    it "character Failure2" $
        parse character "" "'ab'" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid character literal")

stringTest :: SpecWith (Arg Expectation)
stringTest = do
    it "string" $
        parse stringLiteral "" "\"abc\"" `shouldParse` "abc"
    it "stringUTF8" $
        parse stringLiteral "" "\"abc€\"" `shouldParse` "abc€"
    it "stringEscape" $
        parse stringLiteral "" "\"abc\\\"\"" `shouldParse` "abc\""
    it "string Failure" $
        parse stringLiteral "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid string literal")
    it "string Failure2" $
        parse stringLiteral "" "\"abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid string literal")

numberTest :: SpecWith (Arg Expectation)
numberTest = do
    it "number" $
        parse integralLiteral "" "123" `shouldParse` 123
    it "binaryIntegral" $
        parse integralLiteral "" "0b101" `shouldParse` 5
    it "hexIntegral" $
        parse integralLiteral "" "0x1f" `shouldParse` 31
    it "octalIntegral" $
        parse integralLiteral "" "0o37" `shouldParse` 31
    it "floatIntegral" $
        parse floatLiteral "" "1.23" `shouldParse` 1.23
    it "floatIntegral2" $
        parse floatLiteral "" "0.23" `shouldParse` 0.23
    it "number Failure" $
        parse integralLiteral "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")
    it "number Failure2" $
        parse integralLiteral "" "0b2" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")
    it "number Failure3" $
        parse integralLiteral "" "0xg" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")
    it "number Failure4" $
        parse integralLiteral "" "0o8" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")
    it "number Failure5" $
        parse floatLiteral "" "1.23e" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")
    it "number Failure6" $
        parse floatLiteral "" "1.23E" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")
    it "number Failure7" $
        parse (floatLiteral <* eof) "" "0.2.3" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid number literal")

booleanTest :: SpecWith (Arg Expectation)
booleanTest = do
    it "boolean" $
        parse boolean "" "true" `shouldParse` True
    it "boolean2" $
        parse boolean "" "false" `shouldParse` False
    it "boolean Failure" $
        parse boolean "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid boolean literal")

literalTest :: SpecWith (Arg Expectation)
literalTest = do
    it "literal" $
        parse literal "" "123" `shouldParse` Integral 123
    it "literalFloat" $
        parse literal "" "12.3" `shouldParse` Floating 12.3
    it "literal2" $
        parse literal "" "\"abc\"" `shouldParse` String "abc"
    it "literal3" $
        parse literal "" "true" `shouldParse` Boolean True
    it "literal4" $
        parse literal "" "false" `shouldParse` Boolean False
    it "literal5" $
        parse nullLiteral "" "null" `shouldParse` Null
    it "literal Failure" $
        parse literal "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid literal")
    it "literal Failure2" $
        parse literal "" "\"abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid literal")

keywordTest :: SpecWith (Arg Expectation)
keywordTest = do
    it "keyword If" $
        parse (keyword "if") "" "if" `shouldParse` "if"
    it "keyword Else" $
        parse (keyword "else") "" "else" `shouldParse` "else"
    it "keyword While" $
        parse (keyword "while") "" "while" `shouldParse` "while"
    it "keyword Return" $
        parse (keyword "return") "" "return" `shouldParse` "return"
    it "keyword Null" $
        parse (keyword "null") "" "null" `shouldParse` "null"
    it "keyword True" $
        parse (keyword "true") "" "true" `shouldParse` "true"
    it "keyword False" $
        parse (keyword "false") "" "false" `shouldParse` "false"
    it "keyword Failure" $
        parse (keyword "if") "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected 'if' keyword")

typeTest :: SpecWith (Arg Expectation)
typeTest = do
    it "Character type" $
        parse bundledType "" "Character" `shouldParse` "Character"
    it "String type" $
        parse bundledType "" "String" `shouldParse` "String"
    it "Short type" $
        parse bundledType "" "Short" `shouldParse` "Short"
    it "Integer type" $
        parse bundledType "" "Integer" `shouldParse` "Integer"
    it "Long type" $
        parse bundledType "" "Long" `shouldParse` "Long"
    it "Float type" $
        parse bundledType "" "Float" `shouldParse` "Float"
    it "Double type" $
        parse bundledType "" "Double" `shouldParse` "Double"
    it "Boolean type" $
        parse bundledType "" "Boolean" `shouldParse` "Boolean"
    it "type Failure" $
        parse bundledType "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected type")

identifierTest :: SpecWith (Arg Expectation)
identifierTest = do
    it "identifier" $
        parse identifier "" "abc" `shouldParse` "abc"
    it "identifier2" $
        parse identifier "" "abc123" `shouldParse` "abc123"
    it "identifier3" $
        parse identifier "" "abc_123" `shouldParse` "abc_123"
    it "identifier Failure" $
        parse identifier "" "123abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid identifier")
    it "identifier Failure2" $
        parse identifier "" "true" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid identifier")
    it "identifier Failure3" $
        parse identifier "" "while" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid identifier")
    it "identifier Failure4" $
        parse identifier "" "Integer" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid identifier")

operatorTest :: SpecWith (Arg Expectation)
operatorTest = do
    it "bitwise unary ~" $
        parse unaryOperator "" "~" `shouldParse` (Operator Bitwise Unary "~")
    it "bitwise &" $
        parse binaryOperator "" "&" `shouldParse` (Operator Bitwise Binary "&")
    it "bitwise |" $
        parse binaryOperator "" "|" `shouldParse` (Operator Bitwise Binary "|")
    it "bitwise ^" $
        parse binaryOperator "" "^" `shouldParse` (Operator Bitwise Binary "^")
    it "bitwise <<" $
        parse binaryOperator "" "<<" `shouldParse` (Operator Bitwise Binary "<<")
    it "bitwise >>" $
        parse binaryOperator "" ">>" `shouldParse` (Operator Bitwise Binary ">>")
    it "bitwise <<<" $
        parse binaryOperator "" "<<<" `shouldParse` (Operator Bitwise Binary "<<<")
    it "bitwise >>>" $
        parse binaryOperator "" ">>>" `shouldParse` (Operator Bitwise Binary ">>>")
    it "arithmetic unary +" $
        parse unaryOperator "" "+" `shouldParse` (Operator Arithmetic Unary "+")
    it "arithmetic unary -" $
        parse unaryOperator "" "-" `shouldParse` (Operator Arithmetic Unary "-")
    it "arithmetic +" $
        parse binaryOperator "" "+" `shouldParse` (Operator Arithmetic Binary "+")
    it "arithmetic -" $
        parse binaryOperator "" "-" `shouldParse` (Operator Arithmetic Binary "-")
    it "arithmetic *" $
        parse binaryOperator "" "*" `shouldParse` (Operator Arithmetic Binary "*")
    it "arithmetic /" $
        parse binaryOperator "" "/" `shouldParse` (Operator Arithmetic Binary "/")
    it "arithmetic %" $
        parse binaryOperator "" "%" `shouldParse` (Operator Arithmetic Binary "%")
    it "arithmetic **" $
        parse binaryOperator "" "**" `shouldParse` (Operator Arithmetic Binary "**")
    it "logical unary !" $
        parse unaryOperator "" "!" `shouldParse` (Operator Logical Unary "!")
    it "logical &&" $
        parse binaryOperator "" "&&" `shouldParse` (Operator Logical Binary "&&")
    it "logical ||" $
        parse binaryOperator "" "||" `shouldParse` (Operator Logical Binary "||")
    it "logical ^|" $
        parse binaryOperator "" "^|" `shouldParse` (Operator Logical Binary "^|")
    it "comparison ==" $
        parse binaryOperator "" "==" `shouldParse` (Operator Comparison Binary "==")
    it "comparison !=" $
        parse binaryOperator "" "!=" `shouldParse` (Operator Comparison Binary "!=")
    it "comparison <" $
        parse binaryOperator "" "<" `shouldParse` (Operator Comparison Binary "<")
    it "comparison <=" $
        parse binaryOperator "" "<=" `shouldParse` (Operator Comparison Binary "<=")
    it "comparison >" $
        parse binaryOperator "" ">" `shouldParse` (Operator Comparison Binary ">")
    it "comparison >=" $
        parse binaryOperator "" ">=" `shouldParse` (Operator Comparison Binary ">=")
    it "assignment =" $
        parse binaryOperator "" "=" `shouldParse` (Operator Assignment Binary "=")
    it "assignment +=" $
        parse binaryOperator "" "+=" `shouldParse` (Operator Assignment Binary "+=")
    it "assignment -=" $
        parse binaryOperator "" "-=" `shouldParse` (Operator Assignment Binary "-=")
    it "assignment *=" $
        parse binaryOperator "" "*=" `shouldParse` (Operator Assignment Binary "*=")
    it "assignment /=" $
        parse binaryOperator "" "/=" `shouldParse` (Operator Assignment Binary "/=")
    it "assignment %=" $
        parse binaryOperator "" "%=" `shouldParse` (Operator Assignment Binary "%=")
    it "assignment **=" $
        parse binaryOperator "" "**=" `shouldParse` (Operator Assignment Binary "**=")
    it "assignment &=" $
        parse binaryOperator "" "&=" `shouldParse` (Operator Assignment Binary "&=")
    it "assignment |=" $
        parse binaryOperator "" "|=" `shouldParse` (Operator Assignment Binary "|=")
    it "assignment ^=" $
        parse binaryOperator "" "^=" `shouldParse` (Operator Assignment Binary "^=")
    it "assignment <<=" $
        parse binaryOperator "" "<<=" `shouldParse` (Operator Assignment Binary "<<=")
    it "assignment >>=" $
        parse binaryOperator "" ">>=" `shouldParse` (Operator Assignment Binary ">>=")
    it "assignment >>>=" $
        parse binaryOperator "" ">>>=" `shouldParse` (Operator Assignment Binary ">>>=")
    it "assignment <<<=" $
        parse binaryOperator "" "<<<=" `shouldParse` (Operator Assignment Binary "<<<=")
    it "update pre ++" $
        parse updatePreUnaryOperator "" "++" `shouldParse` (PrePostOperator False Update Unary "++")
    it "update pre --" $
        parse updatePreUnaryOperator "" "--" `shouldParse` (PrePostOperator False Update Unary "--")
    it "update post ++" $
        parse updatePostUnaryOperator "" "++" `shouldParse` (PrePostOperator True Update Unary "++")
    it "update post --" $
        parse updatePostUnaryOperator "" "--" `shouldParse` (PrePostOperator True Update Unary "--")
    it "operator Failure" $
        parse binaryOperator "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid binary operator")
    it "operator Failure2" $
        parse unaryOperator "" "abc" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "invalid unary operator")

simpleLiteralExpressionTest :: SpecWith (Arg Expectation)
simpleLiteralExpressionTest = do
    it "integral" $
        parse expression "" "123" `shouldParse` (LiteralExpression (Integral 123))
    it "floating" $
        parse expression "" "12.3" `shouldParse` (LiteralExpression (Floating 12.3))
    it "string" $
        parse expression "" "\"abc\"" `shouldParse` (LiteralExpression (String "abc"))
    it "boolean" $
        parse expression "" "true" `shouldParse` (LiteralExpression (Boolean True))
    it "character" $
        parse expression "" "'a'" `shouldParse` (LiteralExpression (Character 'a'))

simpleIdentifierExpressionTest :: SpecWith (Arg Expectation)
simpleIdentifierExpressionTest = do
    it "identifier" $
        parse expression "" "abc" `shouldParse` (IdentifierExpression "abc")

simpleOperatorExpressionTest :: SpecWith (Arg Expectation)
simpleOperatorExpressionTest = do
    it "unary operator" $
        parse expression "" "-123" `shouldParse` (UnaryExpression (Operator Arithmetic Unary "-") (LiteralExpression (Integral 123)))
    it "unary operator2" $
        parse expression "" "!true" `shouldParse` (UnaryExpression (Operator Logical Unary "!") (LiteralExpression (Boolean True)))
    it "binary operator" $
        parse expression "" "123 + 456" `shouldParse` (BinaryExpression (LiteralExpression (Integral 123)) (Operator Arithmetic Binary "+") (LiteralExpression (Integral 456)))
    it "binary operator2" $
        parse expression "" "kew <<< hub" `shouldParse` (BinaryExpression (IdentifierExpression "kew") (Operator Bitwise Binary "<<<") (IdentifierExpression "hub"))

simpleFunctionCallExpressionTest :: SpecWith (Arg Expectation)
simpleFunctionCallExpressionTest = do
    it "function call" $
        parse expression "" "abc()" `shouldParse` (CallExpression $ Call "abc" [])
    it "function call2" $
        parse expression "" "abc(123)" `shouldParse` (CallExpression $ Call "abc" [(LiteralExpression (Integral 123))])
    it "function call3" $
        parse expression "" "abc(123, 456)" `shouldParse` (CallExpression $ Call "abc" [(LiteralExpression (Integral 123)), (LiteralExpression (Integral 456))])

complexOperatorExpressionTest :: SpecWith (Arg Expectation)
complexOperatorExpressionTest = do
    it "!(4 + 5 * (2 - a))" $
        parse expression "" "!(4 + 5 * (2 - a))" `shouldParse` (UnaryExpression (Operator Logical Unary "!") $ FoldedExpression $ (BinaryExpression (LiteralExpression (Integral 4)) (Operator Arithmetic Binary "+") (BinaryExpression (LiteralExpression (Integral 5)) (Operator Arithmetic Binary "*") $ FoldedExpression $ (BinaryExpression (LiteralExpression (Integral 2)) (Operator Arithmetic Binary "-") (IdentifierExpression "a")))))
    it "a + b * c + d" $
        parse expression "" "a + b * c + d" `shouldParse` (BinaryExpression (IdentifierExpression "a") (Operator Arithmetic Binary "+") (BinaryExpression (IdentifierExpression "b") (Operator Arithmetic Binary "*") (BinaryExpression (IdentifierExpression "c") (Operator Arithmetic Binary "+") (IdentifierExpression "d"))))
    it "hub <<< (n & ~(mask >> 1))" $
        parse expression "" "hub <<< (n & ~(mask >> 1))" `shouldParse` (BinaryExpression (IdentifierExpression "hub") (Operator Bitwise Binary "<<<") $ FoldedExpression $ (BinaryExpression (IdentifierExpression "n") (Operator Bitwise Binary "&") (UnaryExpression (Operator Bitwise Unary "~") $ FoldedExpression $ (BinaryExpression (IdentifierExpression "mask") (Operator Bitwise Binary ">>") (LiteralExpression (Integral 1))))))

complexFunctionCallExpressionTest :: SpecWith (Arg Expectation)
complexFunctionCallExpressionTest = do
    it "abc(123 + a, -factorial(4))" $
        parse expression "" "abc(123 + a, -factorial(4))" `shouldParse` (CallExpression $ Call "abc" [(BinaryExpression (LiteralExpression (Integral 123)) (Operator Arithmetic Binary "+") (IdentifierExpression "a")), (UnaryExpression (Operator Arithmetic Unary "-") $ CallExpression $ Call "factorial" [(LiteralExpression (Integral 4))])])
    it "abc((125 + 2) * 4)" $
        parse expression "" "abc((125 + 2) * 4)" `shouldParse` (CallExpression $ Call "abc" [BinaryExpression (FoldedExpression (BinaryExpression (LiteralExpression (Integral 125)) (Operator Arithmetic Binary "+") (LiteralExpression (Integral 2)))) (Operator Arithmetic Binary "*") (LiteralExpression (Integral 4))])

updateExpressionTest :: SpecWith (Arg Expectation)
updateExpressionTest = do
    it "++a" $
        parse expression "" "++a" `shouldParse` (UnaryExpression (PrePostOperator False Update Unary "++") (IdentifierExpression "a"))
    it "a++" $
        parse expression "" "a++" `shouldParse` (UnaryExpression (PrePostOperator True Update Unary "++") (IdentifierExpression "a"))
    it "--a" $
        parse expression "" "--a" `shouldParse` (UnaryExpression (PrePostOperator False Update Unary "--") (IdentifierExpression "a"))
    it "a--" $
        parse expression "" "a--" `shouldParse` (UnaryExpression (PrePostOperator True Update Unary "--") (IdentifierExpression "a"))
    it "(4 * 2)--" $
        parse expression "" "(4 * 2)--" `shouldParse` (UnaryExpression (PrePostOperator True Update Unary "--") $ FoldedExpression $ (BinaryExpression (LiteralExpression (Integral 4)) (Operator Arithmetic Binary "*") (LiteralExpression (Integral 2))))
    it "++(4 * 2)" $
        parse expression "" "++(4 * 2)" `shouldParse` (UnaryExpression (PrePostOperator False Update Unary "++") $ FoldedExpression $ (BinaryExpression (LiteralExpression (Integral 4)) (Operator Arithmetic Binary "*") (LiteralExpression (Integral 2))))
    it "abc()++" $
        parse expression "" "abc()++" `shouldParse` (UnaryExpression (PrePostOperator True Update Unary "++") (CallExpression $ Call "abc" []))
    it "++abc()" $
        parse expression "" "++abc()" `shouldParse` (UnaryExpression (PrePostOperator False Update Unary "++") (CallExpression $ Call "abc" []))

invalidExpressionTest :: SpecWith (Arg Expectation)
invalidExpressionTest = do
    it "a -" $
        parse expression "" "a -" `shouldFailWith` errFancy 3 (fancy $ ErrorFail "expected expression")
    it "a + b -" $
        parse expression "" "a + b -" `shouldFailWith` errFancy 7 (fancy $ ErrorFail "expected expression")
    it "abc(45" $
        parse expression "" "abc(45" `shouldFailWith` errFancy 6 (fancy $ ErrorFail "expected ')'")
    it "(4 + 5) * (2 - 4" $
        parse expression "" "(4 + 5) * (2 - 4" `shouldFailWith` errFancy 16 (fancy $ ErrorFail "expected ')'")

variableDeclarationTest :: SpecWith (Arg Expectation)
variableDeclarationTest = do
    it "Integer a" $
        parse variableDeclaration "" "Integer a" `shouldParse` (VariableDeclaration "Integer" "a" Nothing)
    it "Integer a = 123" $
        parse variableDeclaration "" "Integer a = 123" `shouldParse` (VariableDeclaration "Integer" "a" (Just (LiteralExpression (Integral 123))))
    it "Integer a = 123 + 456" $
        parse variableDeclaration "" "Integer a = 123 + 456" `shouldParse` (VariableDeclaration "Integer" "a" (Just (BinaryExpression (LiteralExpression (Integral 123)) (Operator Arithmetic Binary "+") (LiteralExpression (Integral 456)))))
    it "Integer a = null" $
        parse variableDeclaration "" "Integer a = null" `shouldParse` (VariableDeclaration "Integer" "a" (Just (LiteralExpression Null)))
    it "Integer = 123" $
        parse variableDeclaration "" "Integer = 123" `shouldFailWith` errFancy 8 (fancy $ ErrorFail "expected identifier")
    it "a = 45" $
        parse variableDeclaration "" "a = 45" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected type")
    it "Integer a = 123 +" $
        parse variableDeclaration "" "Integer a = 123 +" `shouldFailWith` errFancy 17 (fancy $ ErrorFail "expected expression")
    it "Integer a 7" $
        parse variableDeclaration "" "Integer a 7" `shouldFailWith` errFancy 10 (fancy $ ErrorFail "expected '='")

variableDeclarationStatementTest :: SpecWith (Arg Expectation)
variableDeclarationStatementTest = do
    it "Integer a;" $
        parse statement "" "Integer a;" `shouldParse` (DeclarationStatement (VariableDeclaration "Integer" "a" Nothing))
    it "Integer a = 123;" $
        parse statement "" "Integer a = 123;" `shouldParse` (DeclarationStatement (VariableDeclaration "Integer" "a" (Just (LiteralExpression (Integral 123)))))
    it "Integer a = 123 + 456;" $
        parse statement "" "Integer a = 123 + 456;" `shouldParse` (DeclarationStatement (VariableDeclaration "Integer" "a" (Just (BinaryExpression (LiteralExpression (Integral 123)) (Operator Arithmetic Binary "+") (LiteralExpression (Integral 456))))))
    it "Integer a = null;" $
        parse statement "" "Integer a = null;" `shouldParse` (DeclarationStatement (VariableDeclaration "Integer" "a" (Just (LiteralExpression Null))))
    it "Integer = 123;" $
        parse statement "" "Integer = 123;" `shouldFailWith` errFancy 8 (fancy $ ErrorFail "expected identifier")
    it "Integer a = 123 +;" $
        parse statement "" "Integer a = 123 +;" `shouldFailWith` errFancy 17 (fancy $ ErrorFail "expected expression")
    it "Integer a = 7" $
        parse statement "" "Integer a = 7" `shouldFailWith` errFancy 13 (fancy $ ErrorFail "expected ';'")
    it "Integer a 7;" $
        parse statement "" "Integer a 7;" `shouldFailWith` errFancy 10 (fancy $ ErrorFail "expected '='")

expressionStatementTest :: SpecWith (Arg Expectation)
expressionStatementTest = do
    it "a;" $
        parse statement "" "a;" `shouldParse` (ExpressionStatement (IdentifierExpression "a"))
    it "a = 123;" $
        parse statement "" "a = 123;" `shouldParse` (ExpressionStatement (BinaryExpression (IdentifierExpression "a") (Operator Assignment Binary "=") (LiteralExpression (Integral 123))))
    it "a = 123 + 456;" $
        parse statement "" "a = 123 + 456;" `shouldParse` (ExpressionStatement (BinaryExpression (IdentifierExpression "a") (Operator Assignment Binary "=") (BinaryExpression (LiteralExpression (Integral 123)) (Operator Arithmetic Binary "+") (LiteralExpression (Integral 456)))))
    it "a = null;" $
        parse statement "" "a = null;" `shouldParse` (ExpressionStatement (BinaryExpression (IdentifierExpression "a") (Operator Assignment Binary "=") (LiteralExpression Null)))
    it "a++;" $
        parse statement "" "a++;" `shouldParse` (ExpressionStatement (UnaryExpression (PrePostOperator True Update Unary "++") (IdentifierExpression "a")))
    it "a = 123 +;" $
        parse statement "" "a = 123 +;" `shouldFailWith` errFancy 4 (fancy $ ErrorFail "invalid expression")
    it "a = 7" $
        parse statement "" "a = 7" `shouldFailWith` errFancy 5 (fancy $ ErrorFail "expected ';'")
    it "a 7;" $
        parse statement "" "a 7;" `shouldFailWith` errFancy 2 (fancy $ ErrorFail "expected ';'")

blockStatementTest :: SpecWith (Arg Expectation)
blockStatementTest = do
    it "{}" $
        parse statement "" "{}" `shouldParse` (BlockStatement [])
    it "{a;}" $
        parse statement "" "{a;}" `shouldParse` (BlockStatement [(ExpressionStatement (IdentifierExpression "a"))])
    it "{a; b;}" $
        parse statement "" "{a; b;}" `shouldParse` (BlockStatement [(ExpressionStatement (IdentifierExpression "a")), (ExpressionStatement (IdentifierExpression "b"))])
    it "{Integer a = 4; a = 4;   }" $
        parse statement "" "{Integer a = 4; a = 4;   }" `shouldParse` (BlockStatement [(DeclarationStatement (VariableDeclaration "Integer" "a" (Just (LiteralExpression (Integral 4))))), (ExpressionStatement (BinaryExpression (IdentifierExpression "a") (Operator Assignment Binary "=") (LiteralExpression (Integral 4))))])
    it "{{a;}b;}" $
        parse statement "" "{{a;}b;}" `shouldParse` (BlockStatement [(BlockStatement [(ExpressionStatement (IdentifierExpression "a"))]), (ExpressionStatement (IdentifierExpression "b"))])
    it "{Integer a = 4;" $
        parse statement "" "{Integer a = 4;" `shouldFailWith` errFancy 15 (fancy $ ErrorFail "expected '}'")
    it "Integer a = 4;}" $
        parse statement "" "Integer a = 4;}" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected '{'")
    it "{Integer a = 7 +;}" $
        parse statement "" "{Integer a = 7 +;}" `shouldFailWith` errFancy 16 (fancy $ ErrorFail "expected expression")
    it "a;}" $
        parse statement "" "a;}" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected '{'")
    it "{a;}}" $
        parse statement "" "{a;}}" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected '{'")

loopStatementTest :: SpecWith (Arg Expectation)
loopStatementTest = do
    it "while (true) {a;}" $
        parse statement "" "while (true) {a;}" `shouldParse` (LoopStatement (LiteralExpression (Boolean True)) $ BlockStatement [(ExpressionStatement (IdentifierExpression "a"))])
    it "while (true) {a; b;}" $
        parse statement "" "while (true) {a; b;}" `shouldParse` (LoopStatement (LiteralExpression (Boolean True)) $ BlockStatement [(ExpressionStatement (IdentifierExpression "a")), (ExpressionStatement (IdentifierExpression "b"))])
    it "while (a == b) {}" $
        parse statement "" "while (a == b) {}" `shouldParse` (LoopStatement (BinaryExpression (IdentifierExpression "a") (Operator Comparison Binary "==") (IdentifierExpression "b")) $ BlockStatement [])
    it "while(a != b    )    {Integer c = a; a++;}" $
        parse statement "" "while(a != b    )    {Integer c = a; a++;}" `shouldParse` (LoopStatement (BinaryExpression (IdentifierExpression "a") (Operator Comparison Binary "!=") (IdentifierExpression "b")) $ BlockStatement [(DeclarationStatement (VariableDeclaration "Integer" "c" (Just (IdentifierExpression "a")))), (ExpressionStatement (UnaryExpression (PrePostOperator True Update Unary "++") (IdentifierExpression "a")))])
    it "while (a != b) {Integer c = a; a++;" $
        parse statement "" "while (a != b) {Integer c = a; a++;" `shouldFailWith` errFancy 35 (fancy $ ErrorFail "expected '}'")
    it "while (true) {a++}" $
        parse statement "" "while (true) {a++}" `shouldFailWith` errFancy 17 (fancy $ ErrorFail "expected ';'")
    it "while (true) a++;}" $
        parse statement "" "while (true) a++;}" `shouldFailWith` errFancy 13 (fancy $ ErrorFail "expected '{'")
    it "while (true {a++;}" $
        parse statement "" "while (true {a++;}" `shouldFailWith` errFancy 12 (fancy $ ErrorFail "expected ')'")
    it "while (true) {}}" $
        parse statement "" "while (true) {}}" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected '{'")

ifStatementTest :: SpecWith (Arg Expectation)
ifStatementTest = do
    it "if (true) {}" $
        parse statement "" "if (true) {}" `shouldParse` (IfStatement (LiteralExpression (Boolean True)) (BlockStatement []) Nothing)
    it "if (true) {a++;} else {b++;}" $
        parse statement "" "if (true) {a++;} else {b++;}" `shouldParse` (IfStatement (LiteralExpression (Boolean True)) (BlockStatement [(ExpressionStatement (UnaryExpression (PrePostOperator True Update Unary "++") (IdentifierExpression "a")))]) (Just (BlockStatement [(ExpressionStatement (UnaryExpression (PrePostOperator True Update Unary "++") (IdentifierExpression "b")))])))
    it "if (true) {a;} else if (b) {b;}" $
        parse statement "" "if (true) {a;} else if (b) {b;}" `shouldParse` (IfStatement (LiteralExpression (Boolean True)) (BlockStatement [(ExpressionStatement (IdentifierExpression "a"))]) (Just (IfStatement (IdentifierExpression "b") (BlockStatement [(ExpressionStatement (IdentifierExpression "b"))]) Nothing)))
    it "if ((a + b) * 4 == 45) {} else if (denis() == true) {} else {}" $
        parse statement "" "if ((a + b) * 4 == 45) {} else if (denis() == true) {} else {}" `shouldParse` (IfStatement (BinaryExpression (FoldedExpression (BinaryExpression (IdentifierExpression "a") (Operator Arithmetic Binary "+") (IdentifierExpression "b"))) (Operator Arithmetic Binary "*") (BinaryExpression (LiteralExpression (Integral 4)) (Operator Comparison Binary "==") (LiteralExpression (Integral 45)))) (BlockStatement []) (Just (IfStatement (BinaryExpression (CallExpression $ Call "denis" []) (Operator Comparison Binary "==") (LiteralExpression (Boolean True))) (BlockStatement []) (Just (BlockStatement [])))))
    it "if (a + b) a;}" $
        parse statement "" "if (a + b) a;}" `shouldFailWith` errFancy 11 (fancy $ ErrorFail "expected '{'")
    it "if (a + b {a;}" $
        parse statement "" "if (a + b {a;}" `shouldFailWith` errFancy 10 (fancy $ ErrorFail "expected ')'")
    it "if (true) {} else" $
        parse statement "" "if (true) {} else" `shouldFailWith` errFancy 17 (fancy $ ErrorFail "expected '{'")
    it "if (true) {} else if (true) a;}" $
        parse statement "" "if (true) {} else if (true) a;}" `shouldFailWith` errFancy 28 (fancy $ ErrorFail "expected '{'")
    it "if (true) {a;}}" $
        parse statement "" "if (true) {a;}}" `shouldFailWith` errFancy 0 (fancy $ ErrorFail "expected '{'")

returnStatementTest :: SpecWith (Arg Expectation)
returnStatementTest = do
    it "return a;" $
        parse statement "" "return a;" `shouldParse` (ReturnStatement (Just $ IdentifierExpression "a"))
    it "return abc(45);" $
        parse statement "" "return abc(45);" `shouldParse` (ReturnStatement (Just $ CallExpression $ Call "abc" [(LiteralExpression (Integral 45))]))
    it "return;" $
        parse statement "" "return;" `shouldParse` (ReturnStatement Nothing)
    it "return a + b;" $
        parse statement "" "return a + b;" `shouldParse` (ReturnStatement (Just $ BinaryExpression (IdentifierExpression "a") (Operator Arithmetic Binary "+") (IdentifierExpression "b")))
    it "return (a + b) * 4;" $
        parse statement "" "return (a + b) * 4;" `shouldParse` (ReturnStatement (Just $ BinaryExpression (FoldedExpression (BinaryExpression (IdentifierExpression "a") (Operator Arithmetic Binary "+") (IdentifierExpression "b"))) (Operator Arithmetic Binary "*") (LiteralExpression (Integral 4))))
    it "return a + b" $
        parse statement "" "return a + b" `shouldFailWith` errFancy 12 (fancy $ ErrorFail "expected ';'")
    it "return a+;" $
        parse statement "" "return a+;" `shouldFailWith` errFancy 9 (fancy $ ErrorFail "expected expression")
    it "return a + b);" $
        parse statement "" "return a + b);" `shouldFailWith` errFancy 12 (fancy $ ErrorFail "expected ';'")

functionDeclarationTest :: SpecWith (Arg Expectation)
functionDeclarationTest = do
    it "Integer main(Integer a) {}" $
        parse functionDeclaration "" "Integer main(Integer a) {}" `shouldParse` (FunctionDeclaration "Integer" "main" [("Integer", "a")] (BlockStatement []))
    it "Integer main(Integer a, String b) {a++; return a;}" $
        parse functionDeclaration "" "Integer main(Integer a, String b) {a++; return a;}" `shouldParse` (FunctionDeclaration "Integer" "main" [("Integer", "a"), ("String", "b")] (BlockStatement [(ExpressionStatement (UnaryExpression (PrePostOperator True Update Unary "++") (IdentifierExpression "a"))), (ReturnStatement (Just (IdentifierExpression "a")))]))
    it "Integer () {}" $
        parse functionDeclaration "" "Integer () {}" `shouldFailWith` errFancy 8 (fancy $ ErrorFail "expected identifier")
    it "Integer main) {}" $
        parse functionDeclaration "" "Integer main) {}" `shouldFailWith` errFancy 12 (fancy $ ErrorFail "expected '('")
    it "Integer main(Integer a {return a;}" $
        parse functionDeclaration "" "Integer main(Integer a {return a;}" `shouldFailWith` errFancy 23 (fancy $ ErrorFail "expected ')'")
    it "Integer main(Integer a) {return a;" $
        parse functionDeclaration "" "Integer main(Integer a) {return a;" `shouldFailWith` errFancy 34 (fancy $ ErrorFail "expected '}'")
    it "Integer main(Integer) {}" $
        parse functionDeclaration "" "Integer main(Integer) {}" `shouldFailWith` errFancy 20 (fancy $ ErrorFail "expected identifier")
    it "Integer main(a) {}" $
        parse functionDeclaration "" "Integer main(a) {}" `shouldFailWith` errFancy 13 (fancy $ ErrorFail "expected type")
    it "Integer main(Integer a) {return a}" $
        parse functionDeclaration "" "Integer main(Integer a) {return a}" `shouldFailWith` errFancy 33 (fancy $ ErrorFail "expected ';'")

ravenTest :: SpecWith (Arg Expectation)
ravenTest = do
    it "empty" $
        parse ravenParser "" "" `shouldParse` []
    it "just main" $
        parse ravenParser "" "Integer main() {}" `shouldParse` [(FunctionDeclaration "Integer" "main" [] (BlockStatement []))]
    it "double function" $
        parse ravenParser "" "Integer main(Integer a) {return double(4);} Integer double(Integer a) {return a * 2;}" `shouldParse` [(FunctionDeclaration "Integer" "main" [("Integer", "a")] (BlockStatement [(ReturnStatement (Just $ CallExpression (Call "double" [(LiteralExpression (Integral 4))])))])), (FunctionDeclaration "Integer" "double" [("Integer", "a")] (BlockStatement [(ReturnStatement (Just $ BinaryExpression (IdentifierExpression "a") (Operator Arithmetic Binary "*") (LiteralExpression (Integral 2))))]))]

main :: IO ()
main = hspec $
    describe "ravenParser" $ do
        describe "literalValue" $ do
            characterTest
            stringTest
            numberTest
            booleanTest
        describe "literal" $ do
            literalTest
        describe "misc" $ do
            describe "keyword" $ do
                keywordTest
            describe "type" $ do
                typeTest
        describe "identifier" $ do
            identifierTest
        describe "operator" $ do
            operatorTest
        describe "expression" $ do
            describe "simple literal expression" $ do
                simpleLiteralExpressionTest
            describe "simple identifier expression" $ do
                simpleIdentifierExpressionTest
            describe "simple operator expression" $ do
                simpleOperatorExpressionTest
            describe "simple function call expression" $ do
                simpleFunctionCallExpressionTest
            describe "complex operator expression" $ do
                complexOperatorExpressionTest
            describe "complex function call expression" $ do
                complexFunctionCallExpressionTest
            describe "update expression" $ do
                updateExpressionTest
            describe "invalid expression" $ do
                invalidExpressionTest
        describe "variable declaration" $ do
            variableDeclarationTest
        describe "statement" $ do
            variableDeclarationStatementTest
            expressionStatementTest
            blockStatementTest
            loopStatementTest
            ifStatementTest
            returnStatementTest
        describe "function declaration" $ do
            functionDeclarationTest
        describe "Raven" $
            ravenTest
