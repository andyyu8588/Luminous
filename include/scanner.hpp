/*
This is the scanner header file used to tokenize Luminous code.
Created by Yun Ze Zhou and Andy Yu.
*/

#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.hpp"

class Scanner {
  const std::string* code;
  std::vector<Token> tokens;
  std::unordered_map<std::string, TokenType> keywords = {
      {"equals", TOKEN_EQ},
      {"and", TOKEN_AND},
      {"or", TOKEN_OR},
      {"not", TOKEN_NOT},
      {"if", TOKEN_IF},
      {"else", TOKEN_ELSE},
      {"while", TOKEN_WHILE},
      {"return", TOKEN_RETURN},
      {"print", TOKEN_PRINT},
      {"addr", TOKEN_ADDR},
      {"at", TOKEN_AT},
      {"true", TOKEN_TRUE},
      {"false", TOKEN_FALSE},
      {"null", TOKEN_NULL},
      {"for", TOKEN_FOR},
      {"from", TOKEN_FROM},
      {"to", TOKEN_TO},
      {"by", TOKEN_BY},
      {"function", TOKEN_FUNCTION},
      {"class", TOKEN_CLASS},
      {"this", TOKEN_THIS},
      {"inherits", TOKEN_INHERITS},
      {"super", TOKEN_SUPER}};
  int start = 0;
  int current = 0;
  int line = 1;
  size_t curToken = 0;

  // return true if the scanner reached the end of file
  bool isAtEnd();

  // return true if c is a number
  bool isNumber(char c);

  // return true if c is an alphabet or underscore
  bool isAlphabet(char c);

  // scan single character token
  void scanToken();

  // feed in next character in source code
  char nextChar();

  void addToken(TokenType type);

  void addToken(TokenType type, std::string lexeme);

  // consume current character and return true if current character is the
  // character we expect
  bool match(char expected);

  // peek the current character in source code (this does not advance the
  // counter)
  char peek();

  // peek the next characer in the source code (this does not advance the
  // counter)
  char lookAhead();

  // string handler
  void string();

  // number handler
  void number();

  // identifier and keyword handler
  void id();

 public:
  Scanner();

  void tokenize();

  // reset the scanner:
  void reset(const std::string& code);

  const Token& getNextToken();
};
