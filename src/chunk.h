#pragma once
#include <cstdint>
#include <vector>

#include "value.h"

enum OpCode {
  OP_CONSTANT,
  OP_NULL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBSTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_RETURN
};

struct ByteCode {
  const uint8_t code;
  const unsigned int line;

  ByteCode(uint8_t code, unsigned int line);
};

class Chunk {
 private:
  std::vector<ByteCode> bytecode;
  std::vector<Value> constants;
  size_t PC = 0;

 public:
  // bytecode vector getters and setters:
  size_t getBytecodeSize() const;

  ByteCode getBytecodeAt(size_t index) const;

  void addBytecode(uint8_t byte, unsigned int line);

  // constants vector getters and setters:
  size_t getConstantsSize() const;

  Value getConstantAt(size_t index) const;

  size_t addConstant(Value value);  // returns the index in the vector

  ByteCode getBytecodeAtPC();  // increments PC by 1

  ByteCode getPrevBytecode() const;
};
