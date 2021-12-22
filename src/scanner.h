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

#include "token.h"

class Scanner {
  std::string code;
  std::vector<std::shared_ptr<Token>> tokens;
  std::unordered_map<std::string, TokenType> keywords = {
      {"equals", TokenType::EQ},   {"and", TokenType::AND},
      {"or", TokenType::OR},       {"not", TokenType::NOT},
      {"if", TokenType::IF},       {"else", TokenType::ELSE},
      {"while", TokenType::WHILE}, {"return", TokenType::RETURN},
      {"print", TokenType::PRINT}, {"addr", TokenType::ADDR},
      {"at", TokenType::AT}};
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
  Scanner(const std::string& code);
  void tokenize();

  // reset the scanner:
  void reset(std::string code);

  std::shared_ptr<Token> getNextToken();
};
