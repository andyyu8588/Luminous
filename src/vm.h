#pragma once
#include <memory>

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
  InterpretResult interpret(const std::string& source);
};