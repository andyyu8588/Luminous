#pragma once
#include <string>

enum TokenType {

  // Single symbol tokens:
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACK,
  RBRACK,
  BECOMES,
  DOT,
  MINUS,
  PLUS,
  COMMA,
  STAR,
  SLASH,
  SEMI,

  // Single and double symbol tokens:
  LT,
  GT,
  LE,
  GE,

  // Literals:
  ID,
  NUM,
  STRING,

  // Keywords:
  EQ,
  AND,
  OR,
  NOT,
  IF,
  ELSE,
  WHILE,
  RETURN,
  PRINT,
  ADDR,
  AT,

};

class Token {
 public:
  TokenType type;
  std::string lexeme;
  int line;

  Token(TokenType type, std::string lexeme, int line) {
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
  }
};
