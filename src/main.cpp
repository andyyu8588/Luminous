#include <iostream>

#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  VM vm;
  Chunk chunk;
  size_t index = chunk.addConstant(1.2);
  chunk.addBytecode(OP_CONSTANT, 123);
  chunk.addBytecode(index, 123);
  chunk.addBytecode(OP_RETURN, 123);
  disassembleChunk(chunk, "test chunk");
  return 0;
}
