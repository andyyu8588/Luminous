#pragma once
#include <functional>
#include <unordered_set>

#include "chunk.h"
#include "object.h"
#include "scanner.h"

extern bool errorOccured;

using namespace std::placeholders;

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

using ParseFunction = std::function<void(bool)>;

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
       {std::bind(&Compiler::grouping, this, _1), nullptr, PREC_NONE}},
      {TOKEN_RPAREN, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_LBRACE, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_RBRACE, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_COMMA, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_DOT, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_MINUS,
       {std::bind(&Compiler::unary, this, _1),
        std::bind(&Compiler::binary, this, _1), PREC_TERM}},
      {TOKEN_PLUS,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_TERM}},
      {TOKEN_SEMI, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_SLASH,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_FACTOR}},
      {TOKEN_STAR,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_FACTOR}},
      {TOKEN_LT,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_COMPARISON}},
      {TOKEN_GT,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_COMPARISON}},
      {TOKEN_LE,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_COMPARISON}},
      {TOKEN_GE,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_COMPARISON}},
      {TOKEN_NUM, {std::bind(&Compiler::number, this, _1), nullptr, PREC_NONE}},
      {TOKEN_EQ,
       {nullptr, std::bind(&Compiler::binary, this, _1), PREC_EQUALITY}},
      {TOKEN_NOT, {std::bind(&Compiler::unary, this, _1), nullptr, PREC_NONE}},
      {TOKEN_TRUE,
       {std::bind(&Compiler::literal, this, _1), nullptr, PREC_NONE}},
      {TOKEN_FALSE,
       {std::bind(&Compiler::literal, this, _1), nullptr, PREC_NONE}},
      {TOKEN_NULL,
       {std::bind(&Compiler::literal, this, _1), nullptr, PREC_NONE}},
      {TOKEN_STRING,
       {std::bind(&Compiler::string, this, _1), nullptr, PREC_NONE}},
      {TOKEN_ID,
       {std::bind(&Compiler::variable, this, _1), nullptr, PREC_NONE}},
      {TOKEN_AND, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_ELSE, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_IF, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_WHILE, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_OR, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_BECOMES, {nullptr, nullptr, PREC_NONE}},
      {TOKEN_EOF, {nullptr, nullptr, PREC_NONE}}};
  std::unordered_set<std::shared_ptr<ObjectString>, ObjectString::Hash,
                     ObjectString::Comparator>
      existingStrings;

  // advance to the next token in the stream
  void advance();

  // advance with type checking
  void consume(TokenType type, const std::string& message);

  // returns true if the current token matches the given type and advances
  bool match(TokenType type);

  // add given byte to the current chunk
  void emitByte(uint8_t byte);

  // expression parsing functions:
  void expression();

  void parsePrecendence(Precedence precedence);

  // making a constant opcode and pushes it to the Chunk
  uint8_t makeConstant(Value number);

  // parsing functions:
  void binary(bool canAssign);
  void grouping(bool canAssign);
  void literal(bool canAssign);
  void number(bool canAssign);
  void string(bool canAssign);
  void unary(bool canAssign);
  void variable(bool canAssign);

  ParseRule* getRule(TokenType type);

  void declaration();
  void statement();

  void printStatement();
  void expressionStatement();

  // for variable assignment and retrieval
  uint8_t identifierConstant(std::shared_ptr<Token> var);
  void namedVariable(std::shared_ptr<Token> name, bool canAssign);

  // for error synchronization:
  void synchronize();

  // previously used functions for global variable declaration
  // (OP_DEFINE_GLOBAL) void varDeclaration(); uint8_t parseVariable(std::string
  // message);

 public:
  void compile(const std::string& code);
  std::unique_ptr<Chunk> getCurrentChunk();  // std::move currentChunk

  Compiler();
};
