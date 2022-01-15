#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "value.hpp"

class Chunk;
struct Token;

void printChunk(Chunk& chunk, const std::string& name);
size_t printInstruction(Chunk& chunk, size_t index);
void printTokens(const std::vector<Token>& tokens);
void printStack(std::stack<Value>& memory);
