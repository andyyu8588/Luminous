#include "chunk.hpp"

ByteCode::ByteCode(uint8_t code, unsigned int line) : code{code}, line{line} {}

size_t Chunk::getBytecodeSize() const { return bytecode.size(); }

ByteCode Chunk::getBytecodeAt(size_t index) const { return bytecode[index]; }

void Chunk::addBytecode(uint8_t byte, unsigned int line) {
  bytecode.emplace_back(byte, line);
}

void Chunk::modifyCodeAt(uint8_t newCode, int index) {
  bytecode[index].code = newCode;
}

size_t Chunk::getConstantsSize() const { return constants.size(); }

Value Chunk::getConstantAt(size_t index) const { return constants[index]; }

size_t Chunk::addConstant(Value value) {
  constants.emplace_back(value);
  return constants.size() - 1;
}
