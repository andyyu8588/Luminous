#pragma once
#include <string>

enum TokenType {

  // Single symbol tokens:
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_LBRACK,
  TOKEN_RBRACK,
  TOKEN_BECOMES,
  TOKEN_DOT,
  TOKEN_MINUS,
  TOKEN_PLUS,
  TOKEN_COMMA,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_SEMI,

  // Single and double symbol tokens:
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_LE,
  TOKEN_GE,

  // Literals:
  TOKEN_ID,
  TOKEN_NUM,
  TOKEN_STRING,

  // Keywords:
  TOKEN_EQ,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_NOT,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_RETURN,
  TOKEN_PRINT,
  TOKEN_ADDR,
  TOKEN_AT,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NULL,
  TOKEN_FROM,
  TOKEN_TO,
  TOKEN_BY,

  // Misc:
  TOKEN_EOF
};

struct Token {
  const TokenType type;
  const std::string lexeme;
  const int line;

  Token(TokenType type, std::string lexeme, int line)
      : type{type}, lexeme{lexeme}, line{line} {}
};