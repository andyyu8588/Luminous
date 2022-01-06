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

class MemoryStack : public std::stack<Value> {
 public:
  Value getValueAt(size_t index) const;
  void setValueAt(Value value, size_t index);
};

class VM {
 private:
  std::unique_ptr<Chunk> chunk;
  MemoryStack memory;
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
