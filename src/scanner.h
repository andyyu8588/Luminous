/*
This is the scanner header file used to tokenize Luminous code. 
Created by Yun Ze Zhou and Andy Yu.
*/

#pragma once
#include <string>
#include <vector>

/*
Lexical Syntax

Variables
ID     -> string
NUM    -> string of 0-9

Parenthesis
LPAREN -> (
RPAREN -> )
LBRACE -> {
RBRACE -> }
LBRACK -> [
RBRACK -> ]
QUOTE  -> "

Operators
BECOMES-> =
DOT    -> .
COMMA  -> ,
PLUS   -> +
MINUS  -> -
STAR   -> *
SLASH  -> /
SEMI   -> ;

Comparators
EQ     -> equals
AND    -> and
OR     -> or
NOT    -> not
LT     -> <
GT     -> >
LE     -> <=
GE     -> =>

Control flow
IF     -> if
ELSE   -> else
WHILE  -> while
RETURN -> return

Built-in functions
PRINT  -> print
ADDR   -> addr
AT     -> at
*/

/*
Exceptions: 

SFAULT = syntax error
MFAULT = math error

*/

struct Token {
    std::string token;
    std::string value;
};

class Scanner {

public:
    Scanner();
    std::vector<Token> tokenize();
};
