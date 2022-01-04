#pragma once
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "value.h"

class Chunk;
class ObjectString;

enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class VM {
 private:
  std::unique_ptr<Chunk> chunk;
  std::stack<Value> memory;
  std::unordered_map<std::shared_ptr<ObjectString>, Value> globals;

  InterpretResult binaryOperation(char operation);
  InterpretResult run();
  void runtimeError(const char* format, ...);
  void resetMemory();
  bool isFalsey(Value value) const;
  void concatenate(const std::string& c, const std::string& d);

 public:
  InterpretResult interpret(std::unique_ptr<Chunk> chunk);
};
