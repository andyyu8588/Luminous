#pragma once
#include <memory>
#include <stack>

#include "value.h"

class Chunk;

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
 private:
  std::unique_ptr<Chunk> chunk;
  std::stack<Value> memory;

  void binaryOperation(char operation);
  InterpretResult run();

 public:
  InterpretResult interpret(std::unique_ptr<Chunk> chunk);
};
