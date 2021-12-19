#pragma once
#include "chunk.h"
#include "string"

void disassembleChunk(Chunk& chunk, std::string name);
size_t disassembleInstruction(Chunk& chunk, size_t offset);