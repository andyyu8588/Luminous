#include "chunk.h"

ByteCode::ByteCode(uint8_t code, unsigned int line) : code{code}, line{line} {}

size_t Chunk::getBytecodeSize() { return bytecode.size(); }

ByteCode Chunk::getBytecodeAt(size_t index) { return bytecode[index]; }

void Chunk::addBytecode(uint8_t byte, unsigned int line) {
  bytecode.emplace_back(byte, line);
}

double Chunk::getConstantAt(size_t index) { return constants[index]; }

size_t Chunk::addConstant(double constant) {
  constants.emplace_back(constant);
  return constants.size() - 1;
}

ByteCode Chunk::getBytecodeAtPC() { return bytecode[PC++]; }
