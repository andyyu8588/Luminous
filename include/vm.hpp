#pragma once
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "object.hpp"

#define FRAMES_MAX 256

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
  ObjectClosure& closure;
  size_t stackPos;
  size_t PC;
};

class VM {
 private:
  MemoryStack memory;
  std::stack<CallFrame> frames;
  std::unordered_map<std::shared_ptr<ObjectString>, Value, ObjectString::Hash,
                     ObjectString::Comparator>
      globals;

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

  // for calling functions:
  bool callValue(Value callee, int argCount);
  bool call(std::shared_ptr<ObjectClosure> closure, int argCount);

  // for native functions:
  void defineNative(std::string name, NativeFn function);
  static Value clockNative(int argCount, size_t start);

 public:
  InterpretResult interpret(std::shared_ptr<ObjectFunction> function);
  VM();
};
