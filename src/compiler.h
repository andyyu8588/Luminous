#pragma once
#include <functional>
#include <unordered_set>

#include "chunk.h"
#include "object.h"
#include "scanner.h"

extern bool errorOccured;

struct Parser {
  std::shared_ptr<Token> current;
  std::shared_ptr<Token> prev;
};

enum Precedence {
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,
  PREC_COMPARISON,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_CALL,
  PREC_PRIMARY,
};

class Compiler;

using ParseFunction = std::function<void()>;

struct ParseRule {
  ParseFunction prefix;
  ParseFunction infix;
  Precedence precedence;
};

class Compiler {
  Parser parser;
  Scanner scanner;
  std::unique_ptr<Chunk> currentChunk;
  std::unordered_map<TokenType, ParseRule> ruleMap = {
      {TOKEN_LPAREN,
       {std::bind(&Compiler::grouping, this), nullptr, PREC_NONE}},
      {TOKEN_RPAREN, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_MINUS,
       {std::bind(&Compiler::unary, this), std::bind(&Compiler::binary, this),
        PREC_TERM}},
      {TOKEN_PLUS, {nullptr, std::bind(&Compiler::binary, this), PREC_TERM}},
      {TOKEN_STAR, {nullptr, std::bind(&Compiler::binary, this), PREC_FACTOR}},
      {TOKEN_SLASH, {nullptr, std::bind(&Compiler::binary, this), PREC_FACTOR}},
      {TOKEN_LT,
       {nullptr, std::bind(&Compiler::binary, this), PREC_COMPARISON}},
      {TOKEN_GT,
       {nullptr, std::bind(&Compiler::binary, this), PREC_COMPARISON}},
      {TOKEN_LE,
       {nullptr, std::bind(&Compiler::binary, this), PREC_COMPARISON}},
      {TOKEN_GE,
       {nullptr, std::bind(&Compiler::binary, this), PREC_COMPARISON}},
      {TOKEN_NUM, {std::bind(&Compiler::number, this), nullptr, PREC_NONE}},
      {TOKEN_EQ, {nullptr, std::bind(&Compiler::binary, this), PREC_EQUALITY}},
      {TOKEN_NOT, {std::bind(&Compiler::unary, this), nullptr, PREC_NONE}},
      {TOKEN_TRUE, {std::bind(&Compiler::literal, this), nullptr, PREC_NONE}},
      {TOKEN_FALSE, {std::bind(&Compiler::literal, this), nullptr, PREC_NONE}},
      {TOKEN_NULL, {std::bind(&Compiler::literal, this), nullptr, PREC_NONE}},
      {TOKEN_STRING, {std::bind(&Compiler::string, this), nullptr, PREC_NONE}},
      {TOKEN_ID, {std::bind(&Compiler::variable, this), nullptr, PREC_NONE}},
      {TOKEN_EOF, {nullptr, nullptr, PREC_NONE}}};
  std::unordered_set<std::shared_ptr<ObjectString>, ObjectString::Hash,
                     ObjectString::Comparator>
      existingStrings;

  // advance to the next token in the stream
  void advance();

  // advance with type checking
  void consume(TokenType type, const std::string& message);

  // add given byte to the current chunk
  void emitByte(uint8_t byte);

  // for NUM token type and expressions:
  void expression();
  void number();
  uint8_t makeConstant(Value number);
  void grouping();

  void unary();
  void binary();
  void literal();
  void string();

  void parsePrecendence(Precedence precedence);

  ParseRule* getRule(TokenType type);

  void declaration();
  void statement();

  void printStatement();
  void expressionStatement();

  bool match(TokenType type);

  void synchronize();

  void varDeclaration();

  uint8_t parseVariable(std::string message);

  uint8_t identifierConstant(std::shared_ptr<Token> var);

  void variable();

  void namedVariable(std::shared_ptr<Token> name);

 public:
  void compile(const std::string& code);
  std::unique_ptr<Chunk> getCurrentChunk();  // std::move currentChunk

  Compiler();
};
