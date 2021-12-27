#pragma once
#include <cstdint>
#include <vector>

enum OpCode {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBSTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
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
  std::vector<double> constants;
  size_t PC = 0;

 public:
  // bytecode vector getters and setters:
  size_t getBytecodeSize();

  ByteCode getBytecodeAt(size_t index);

  void addBytecode(uint8_t byte, unsigned int line);

  // constants vector getters and setters:
  double getConstantAt(size_t index);

  size_t addConstant(double constant);  // returns the index in the vector

  ByteCode getBytecodeAtPC();
};
