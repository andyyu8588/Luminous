#pragma once
#include <memory>
#include <string>
#include <vector>

#include "vm.hpp"

class Chunk;
struct Token;

void printChunk(Chunk& chunk, const std::string& name);
size_t printInstruction(Chunk& chunk, size_t index);
void printTokens(const std::vector<Token>& tokens);
void printStack(MemoryStack& memory);
