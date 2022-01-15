#pragma once
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "object.hpp"

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

struct CallFrame {
  ObjectFunction& function;
  const size_t stackPos;
};

class VM {
 private:
  MemoryStack memory;
  std::stack<CallFrame> frames;
  std::unordered_map<std::shared_ptr<ObjectString>, Value> globals;

  InterpretResult binaryOperation(char operation);
  InterpretResult run();
  void runtimeError(const char* format, ...);
  void resetMemory();
  bool isFalsey(Value value) const;
  void concatenate(const std::string& c, const std::string& d);
  Chunk& getTopChunk();

  // read the next bytecode depending on situation:
  uint8_t readByte();
  Value readConstant();
  uint16_t readShort();

 public:
  InterpretResult interpret(std::shared_ptr<ObjectFunction> function);
};
