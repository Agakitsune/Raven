## Raven Serialization Ruleset

### Literal

> Literal represent direct number, text or boolean

Literal is represented by the following rule:
| Byte | Description                      |
| ---- |:--------------------------------:|
| 1    | The type of the literal          |
| ?    | The literal                      |

The literal byte depends on the literal
| Literal | Byte |
|:-------:| ---- |
| Number | 00 |
| String | 01 |
| Boolean | 02 |
| Character | 03 |
| Null | 04 |

**Number**

Number are written with the following rule:  
| Byte | Description                    |
| ---- |:------------------------------:|
| 8    | The actual number written in big endian |

```c
/*
** 42: 2a 00 00 00 00 00 00 00
**     ^^
**     +- 0x2a = 42
*/
```

___

**String**

String are written just has their are

```c
/*
** "Hello€": 48 65 6C 6C 6F E2 82 AC 00
**           H  e  l  l  o  ^------^ \0
**                             €
*/
```
___

**Boolean**

Boolean are written with the following rule:  
| Byte | Description              |
| ---- |:------------------------:|
| 1    | The value of the boolean |

```c
/*
** true: 01
** false: 00
*/
```
___

**Character**

Character are written with the following rule:
| Byte | Description              |
| ---- |:------------------------:|
| 1    | The value of the character |

```c
/*
** 'a': 61
** '€': E2 82 AC
*/
```

___

**Null Literal**

Null Literal have no payload, just the type

```c
/*
** null: 04
*/
```

___

### Identifier

> An identifier is the name of a variable or a function

```c
int a = 42;
// 'a' is the identifier
int display(void) {}
// 'display' is the identifier
```

Identifiers are written just like string
___

### Type

> A type is similary to an identifier (a type is basically an identifier to a class/structure)  
> Type in Raven cannot be a pointer

```c
int a = 42;
// 'int' is the type
```
___

### Operation

> An operation can be on 2 operand (binary) or on 1 (unary)

```c
a + 42; // binary
-d; // unary
```

> Operation can be of different type too

```c
a << 2; // bitwise operation
a + b; // algebraic operation
a == b; // boolean operation
```

Operation are written with the following rule:  
| Byte | Description                      |
| ---- |:--------------------------------:|
| 1    | The type of the operation        |
| ?    | The first expression             |
| ?    | The second expression (optional) |

The operation byte depends on the operation
| Operator | Byte |
|:--------:| ---- |
| ~ | 00 |
| & | 01 |
| \| | 02 |
| ^ | 03 |
| << | 04 |
| >> | 05 |
| <<< | 06 |
| >>> | 07 |
| - (unary) | 08 |
| + | 09 |
| - | 0A |
| * | 0B |
| / | 0C |
| % | 0D |
| ** | 0E |
| ! | 0F |
| == | 10 |
| != | 11 |
| < | 12 |
| > | 13 |
| <= | 14 |
| >= | 15 |
| && | 16 |
| \|\| | 17 |
| ^\| | 17 |
| ++ (postfix) | 18 |
| -- (postfix) | 19 |
| ++ (prefix) | 1A |
| -- (prefix) | 1B |
| = | 1C |
| += | 1D |
| -= | 1E |
| *= | 1F |
| /= | 20 |
| %= | 21 |
| **= | 22 |
| &= | 23 |
| \|= | 24 |
| ^= | 25 |
| >>= | 26 |
| <<= | 27 |
| >>>= | 28 |
| <<<= | 29 |

___

### Expression

> An expression is either a literal, an identifier, an operation or a function call

Expression are written with the following rule:
| Byte | Description                      |
| ---- |:--------------------------------:|
| 1    | The type of the expression       |
| ?    | The expression                   |

The expression byte depends on the expression
| Expression | Byte |
|:----------:| ---- |
| Literal | 00 |
| Identifier | 01 |
| Operation | 02 |
| Function Call | 03 |
| Fold Expression | 04 |

> Fold Expression is just when an expression is between parenthesis, this is to handle operator precedence

___

### Block

> A block is a group of statement (usually in a function body or a condition)

```c
// Out of the block
{
    // In the block
    int a = 42;
    printf("%d\n", a);
    {
        // In the block in the block
        int b = 69;
        printf("%d\n", b);
    }
}
```

Block are written with the following rule:
| Byte | Description                      |
| ---- |:--------------------------------:|
| 1    | The begin of the block           |
| ?    | The statements                   |
| 1    | The end of the block             |

The byte of the begin and the end of the block is 0x10

___

### Variable Declaration

> Variable Declaration is usually a type followed by an identifier, and sometimes a variable declarator (an expression)

```c
int a = 42;
/* 'int' is the type
** 'a' is the identifier
**
** the variable declarator is '=' followed by an expression (here it's 42)
*/
```

Variable are written with the following rule:  
| Byte | Description                    |
| ---- |:------------------------------:|
| ?    | The type of the variable       |
| ?    | The identifier of the variable |
| 1    | If there is a declarator       |
| ?    | The declarator of the variable |

```c
int a;
/*
** 69 6E 74 00   61 00   00
** i  n  t \0   a \0    +--- without a declarator
*/
int b = 42;
/*
** 69 6E 74 00   62 00   01   00 00 01 01 2a
** i  n  t \0   b \0     |    |  |  +--+--+-- 42
**                       |    |  |
**                       |    |  +--- number literal
**                       |    +--- literal expression
**                       +--- with a declarator
*/
```
___

### Variable Assignment

> Variable Assignment is usually an identifier followed by a variable declarator (an expression)

```c
a = 42;
/* 'a' is the identifier
**
** the variable declarator is '=' followed by an expression (here it's 42)
*/
```

Variable Assignment are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| ?    | The identifier of the variable |
| ?    | The declarator of the variable |

```c
a = 42;
/*
** 61 00   00 00 01 01 2a
** a \0    |  |  +--+--+-- 42
**         |  |
**         |  +--- number literal
**         +--- literal expression
*/
```

___

### Function Declaration

> Function Declaration is usually a type (the return type) followed by an identifier, and then the parameters, and a function body (a block)

```c
int display(char a) {
    printf("%c\n", a);
}
/* 'int' is the return type
** 'display' is the identifier
**
** the parameters are 'char *a'
** the function body is '{ printf("%s\n", a); }'
*/
```

Function are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| ?    | The return type of the function|
| ?    | The identifier of the function |
| 1    | How many parameters the function has |
| ?    | The parameters of the function (type + identifier) |
| ?    | The function body              |

```c
int display(char a) {
    printf("%c\n", a);
}
/*
** 69 6E 74 00   64 69 73 70 6C 61 79 00   01   63 68 61 72 00   10 ... 10   00
** i  n  t \0   d  i  s  p  l  a  y \0     |    c  h  a  r \0     |     +--- end of the block
**                                         |    |                 +--- begin of the block
**                                         |    +--- identifier of the parameter
**                                         +--- number of parameters
**
*/
```
___

### Function Call

> Function Call is an identifier (the function name) followed by the argument

```c
display('h');
/* 'display' is the identifier
** the argument is 'h'
*/
```

Function Call are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| ?    | The identifier of the function |
| 1    | How many arguments the function has |
| ?    | The arguments of the function (expression)  |

```c
display('h');
/*
** 64 69 73 70 6C 61 79 00   01   00 00 01 00 68
** d  i  s  p  l  a  y \0     |    |  | +--+--+-- 'h'
**                            |    |  |
**                            |    |  +--- number literal
**                            |    +--- literal expression
**                            +--- number of argument
*/
```
___

### If Statement

> If Statement is a condition followed by a block  
> there can be alternative block (else if, else)

```c
if (a == 42) {
    printf("a is 42\n");
} else if (a == 21) {
    printf("a is 21\n");
} else {
    printf("a is not 42 nor 21\n");
}
/* the condition is 'a == 42'
** the block is '{ printf("a is 42\n"); }'
** the alternative block is 'else if (a == 21) { printf("a is 21\n"); }'
** the alternative block is 'else { printf("a is not 42 nor 21\n"); }'
*/
```

If Statement are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| ?    | The condition of the if statement (expression) |
| ?    | The block of the if statement  |
| 1    | If there is an alternative block |
| 1    | If there is a condition (optional) |
| ?    | The condition of the alternate statement (only if there is a condition) |
| ?    | The alternative block |

```c
if (a == 42) {
    printf("a is 42\n");
} else if (a == 21) {
    printf("a is 21\n");
} else {
    printf("a is not 42 nor 21\n");
}
/*
** 02    10    01 62 00    00 00 01 01 2a   10 ... 10
** |     |     |  a  \0    |  |  +--+--+-- 42
** |     |     |           |  |
** |     |     |           |  +--- number literal
** |     |     |           +--- literal expression
** |     |     +--- identifier expression
** |     +--- == operator
** +--- operation
**
** 01    01    02    10    01 62 00    00 00 01 01 15   10 ... 10
** |     |     |     |     |  a  \0    |  |  +--+--+-- 21
** |     |     |     |     |           |  |
** |     |     |     |     |           |  +--- number literal
** |     |     |     |     |           +--- literal expression
** |     |     |     |     +--- identifier expression
** |     |     |     +--- == operator
** |     |     +--- operation
** |     +--- there is a condition (else if)
** +--- alternative block
**
** 01    00   10 ... 10
** |     |
** |     +--- no condition (else)
** +--- alternative block
*/
```
___

### While Statement

> While Statement is a condition followed by a block and form a loop

```c
while (a < 42) {
    printf("%d\n", a);
    a++;
}
/* the condition is 'a < 42'
** the block is '{ printf("%d\n", a); a++; }'
*/
```

While Statement are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| ?    | The condition of the while statement (expression) |
| ?    | The block of the while statement  |

```c
while (a < 42) {
    printf("%d\n", a);
    a++;
}
/*
** 02   12    01 61 00    00 00 01 01 2a   10 ... 10
** |    |     |  a  \0    |  |  +--+--+-- 42
** |    |     |           |  |
** |    |     |           |  +--- number literal
** |    |     |           +--- literal expression
** |    |     +--- identifier expression
** |    +--- < operator
** +--- operation
*/
```
___

### Return Statement

> Return Statement is an expression that is returned by a function

```c
int display(char a) {
    return a;
}

/* the expression is 'a'
*/
```

Return Statement are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| 1    | If there is an expression      |
| ?    | The expression of the return statement (expression) |

```c
int display(char a) {
    return a;
}

/*
** 01   01    61 00
** |    |     a  \0
** |    +--- identifier expression
** +--- there is an expression
*/
```

___

### Statement

> A statement is either a variable declaration, a variable assignment, a function call, an if statement or a while statement

Statement are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| 1    | The type of the statement      |
| ?    | The statement                  |

The statement byte depends on the statement
| Statement | Byte |
|:---------:| ---- |
| Block | 00 |
| Expression | 01 |
| Declaration | 02 |
| If Statement | 03 |
| While Statement | 04 |
| Return Statement | 05 |

___

### Declaration

> In Raven, the first thing we deserialize is either a variable declaration or a function declaration

Declaration are written with the following rule:
| Byte | Description                    |
| ---- |:------------------------------:|
| 1    | The type of the declaration    |
| ?    | The declaration                |

The declaration byte depends on the declaration
| Declaration | Byte |
|:-----------:| ---- |
| Variable Declaration | 00 |
| Function Declaration | 01 |

___
