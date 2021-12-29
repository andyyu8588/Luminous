#pragma once
#include <memory>
#include <stack>
#include <string>

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

  InterpretResult binaryOperation(char operation);
  InterpretResult run();
  void runtimeError(const char* format, ...);
  void resetMemory();
  bool isFalsey(Value value);

 public:
  InterpretResult interpret(std::unique_ptr<Chunk> chunk);
};
