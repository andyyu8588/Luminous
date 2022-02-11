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
  Value* getValuePtrAt(size_t index) const;
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
  std::shared_ptr<ObjectUpvalue> openUpvalues = nullptr;  // head of linked list
  const std::shared_ptr<ObjectString> constructorString =
      std::make_shared<ObjectString>("constructor");

  InterpretResult binaryOperation(char operation);
  InterpretResult run();
  void runtimeError(const char* format, ...);
  void resetMemory();
  bool isFalsey(Value value) const;
  void concatenate(const std::string& c, const std::string& d);
  void concatenate(const std::string& c, double d);
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

  // for upvalues:
  std::shared_ptr<ObjectUpvalue> captureUpvalue(Value* local, int localIndex);
  void closeUpvalues(int lastIndex);

  // for classes:
  void defineMethod(std::shared_ptr<ObjectString> name);
  bool bindMethod(const ObjectClass& instanceOf,
                  std::shared_ptr<ObjectString> name);
  bool invoke(std::shared_ptr<ObjectString> name, int argCount);
  bool invokeFromClass(const ObjectClass& instanceOf,
                       std::shared_ptr<ObjectString> name, int argCount);

 public:
  InterpretResult interpret(std::shared_ptr<ObjectFunction> function);
  VM();
};
