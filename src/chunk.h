#pragma once
#include <cstdint>
#include <vector>

#include "value.h"

enum OpCode {
  OP_CONSTANT,
  OP_NULL,
  OP_TRUE,
  OP_FALSE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBSTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_RETURN
};

class ByteCode {
 public:
  uint8_t code;
  unsigned int line;

  ByteCode(uint8_t code, unsigned int line);
};

class Chunk {
 private:
  std::vector<ByteCode> bytecode;
  std::vector<Value> constants;
  size_t PC = 0;

 public:
  // bytecode vector getters and setters:
  size_t getBytecodeSize();

  ByteCode getBytecodeAt(size_t index);

  void addBytecode(uint8_t byte, unsigned int line);

  // constants vector getters and setters:
  Value getConstantAt(size_t index);

  size_t addConstant(Value value);  // returns the index in the vector

  ByteCode getBytecodeAtPC();

  ByteCode getPrevBytecode();
};
