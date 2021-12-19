#include "debug.h"

#include <iomanip>
#include <iostream>

size_t simpleInstruction(std::string name, size_t index) {
  std::cout << name << std::endl;
  return index + 1;
}

size_t constantInstruction(std::string name, Chunk& chunk, size_t index) {
  uint8_t constantIndex = chunk.getBytecodeAt(index).code;
  std::cout << name << " " << chunk.getConstantAt(constantIndex) << std::endl;
  return index + 2;
}

void disassembleChunk(Chunk& chunk, std::string name) {
  std::cout << "== " << name << " ==" << std::endl;

  for (size_t index = 0; index < chunk.getBytecodeSize();) {
    index = disassembleInstruction(chunk, index);
  }
}

size_t disassembleInstruction(Chunk& chunk, size_t index) {
  std::cout << std::setfill('0') << std::setw(5) << index << " ";
  std::cout << std::setfill(' ') << std::setw(5) << chunk.getBytecodeAt(index).line << " ";
  uint8_t instruction = chunk.getBytecodeAt(index).code;
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, index);
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", index);
    default:
      std::cout << "Unkown opcode " << instruction << std::endl;
      return index + 1;
  }
}