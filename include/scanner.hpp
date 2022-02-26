#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "token.hpp"

class Scanner {
  const std::string* code;
  std::vector<std::shared_ptr<Token>> tokens;
  std::string currentFile;

  // for imports:
  const std::string stdPathPrefix = "lib/src/";
  static std::unordered_set<std::string> importedFiles;
  const std::unordered_map<std::string, std::string> stdLibs = {
      {"Queue", "queue.lum"}, {"Stack", "stack.lum"},
      {"Math", "math.lum"},   {"Random", "random.lum"},
      {"Heap", "heap.lum"},   {"HashMap", "hashmap.lum"}};
  const std::unordered_map<std::string, TokenType> keywords = {
      {"equals", TOKEN_EQ},
      {"and", TOKEN_AND},
      {"or", TOKEN_OR},
      {"not", TOKEN_NOT},
      {"if", TOKEN_IF},
      {"else", TOKEN_ELSE},
      {"while", TOKEN_WHILE},
      {"return", TOKEN_RETURN},
      {"print", TOKEN_PRINT},
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
      {"super", TOKEN_SUPER},
      {"private", TOKEN_PRIVATE},
      {"protected", TOKEN_PROTECTED},
      {"public", TOKEN_PUBLIC},
      {"break", TOKEN_BREAK},
      {"continue", TOKEN_CONTINUE}};
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
  void reset(const std::string& code, std::string currentFile);

  const Token* getNextToken();
};
