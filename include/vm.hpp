/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yunze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#pragma once
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

#include "object.hpp"

#define FRAMES_MAX 256

class Chunk;
class ObjectString;

class VMException {};

class MemoryStack : public std::stack<Value> {
 public:
  Value getValueAt(size_t index) const;
  Value* getValuePtrAt(size_t index) const;
  void setValueAt(Value value, size_t index);
};

struct CallFrame {
  ObjectClosure& closure;
  const size_t stackPos;
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

  void binaryOperation(char operation);
  void run();
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
  void callValue(Value callee, int argCount);
  void call(std::shared_ptr<ObjectClosure> closure, int argCount);

  // for native functions:
  void defineNative(std::string name, NativeFn function);
  Value clockNative(int argCount, size_t start);
  Value substringNative(int argCount, size_t start);
  Value sizeNative(int argCount, size_t start);
  Value floorNative(int argCount, size_t start);
  Value ceilNative(int argCount, size_t start);
  Value typeNative(int argCount, size_t start);
  Value throwNative(int argCount, size_t start);

  // for upvalues:
  std::shared_ptr<ObjectUpvalue> captureUpvalue(Value* local, int localIndex);
  void closeUpvalues(int lastIndex);

  // for classes:
  void defineField(std::shared_ptr<ObjectString> name);
  void defineMethod(std::shared_ptr<ObjectString> name);
  bool bindMethod(const ObjectClass& instanceOf,
                  std::shared_ptr<ObjectString> name);
  void invoke(std::shared_ptr<ObjectString> name, int argCount);
  void invokeFromClass(const ObjectClass& instanceOf,
                       std::shared_ptr<ObjectString> name, int argCount);
  void validateAccessModifier(std::shared_ptr<ObjectString> name,
                              ObjectClass& superclass);
  void validateAccessModifier(std::shared_ptr<ObjectString> name,
                              ObjectInstance& instance);

 public:
  void interpret(std::shared_ptr<ObjectFunction> function);
  VM();
};
