#pragma once
#include <functional>

#include "chunk.h"
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
      {TOKEN_NUM, {std::bind(&Compiler::number, this), nullptr, PREC_NONE}},
      {TOKEN_EOF, {nullptr, nullptr, PREC_NONE}}};

  void expression();

  // advance to the next token in the stream
  void advance();

  // advance with type checking
  void consume(TokenType type, const std::string& message);

  // add given byte to the current chunk
  void emitByte(uint8_t byte);

  // for NUM token type and expressions:
  void number();
  uint8_t makeConstant(Value number);
  void grouping();

  void unary();
  void binary();

  void parsePrecendence(Precedence precedence);

  ParseRule* getRule(TokenType type);

 public:
  Compiler();

  std::unique_ptr<Chunk> getCurrentChunk();

  void compile(const std::string& code);
};
