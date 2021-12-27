#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "value.h"

class Chunk;
class Token;

void printChunk(Chunk& chunk);
size_t printInstruction(Chunk& chunk, size_t index);
void printTokens(const std::vector<std::shared_ptr<Token>>& tokens);
void printStack(std::stack<Value>& memory);
