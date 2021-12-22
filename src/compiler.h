#pragma once
#include "chunk.h"
#include "scanner.h"

extern bool errorOccured;

struct Parser {
  std::shared_ptr<Token> current;
  std::shared_ptr<Token> prev;
};

class Compiler {
  Parser parser;
  Scanner scanner;
  std::unique_ptr<Chunk> currentChunk;

  void expression();

  // advance to the next token in the stream
  void advance();

  // advance with type checking
  void consume(TokenType type, const std::string message);

  // add given byte to the current chunk
  void emitByte(uint8_t byte);

  // for NUM token type:
  void number();
  uint8_t makeConstant(double number);

  void grouping();

 public:
  Compiler(const std::string& code);

  std::unique_ptr<Chunk> getCurrentChunk();

  // for recompilation while begin active
  void compile(std::string code);

  void compile();
};
