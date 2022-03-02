/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yunze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#include "chunk.hpp"

ByteCode::ByteCode(uint8_t code, unsigned int line, const std::string& filename)
    : code{code}, line{line}, filename{filename} {}

size_t Chunk::getBytecodeSize() const { return bytecode.size(); }

ByteCode Chunk::getBytecodeAt(size_t index) const { return bytecode[index]; }

void Chunk::addBytecode(uint8_t byte, unsigned int line, std::string filename) {
  bytecode.emplace_back(byte, line, filename);
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
