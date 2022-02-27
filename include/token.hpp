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
  TOKEN_PLUSBECOMES,
  TOKEN_MINUSBECOMES,
  TOKEN_STARBECOMES,
  TOKEN_SLASHBECOMES,
  TOKEN_PERCBECOMES,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_PERC,
  TOKEN_COMMA,
  TOKEN_DOT,
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
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NULL,
  TOKEN_FROM,
  TOKEN_TO,
  TOKEN_BY,
  TOKEN_FUNCTION,
  TOKEN_CLASS,
  TOKEN_THIS,
  TOKEN_INHERITS,
  TOKEN_SUPER,
  TOKEN_PRIVATE,
  TOKEN_PROTECTED,
  TOKEN_PUBLIC,
  TOKEN_BREAK,
  TOKEN_CONTINUE,

  // Misc:
  TOKEN_EOF
};

struct Token {
  const TokenType type;
  const std::string lexeme;
  const int line;
  const std::string file;

  Token(TokenType type, std::string lexeme, int line, std::string file)
      : type{type}, lexeme{lexeme}, line{line}, file{file} {}
};