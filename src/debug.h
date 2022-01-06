#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "value.h"

class Chunk;
struct Token;

void printChunk(Chunk& chunk);
size_t printInstruction(Chunk& chunk, size_t index);
void printTokens(const std::vector<Token>& tokens);
void printStack(std::stack<Value>& memory);
void disassembleChunk(Chunk& chunk);
