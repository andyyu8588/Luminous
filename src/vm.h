#pragma once
#include <memory>

#include "chunk.h"

class Chunk;

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
 private:
  std::unique_ptr<Chunk> chunk;

 public:
  InterpretResult interpret(Chunk& chunk);
  // VM(std::unique_ptr<Chunk> chunk) : chunk{std::move(chunk)} {}
};