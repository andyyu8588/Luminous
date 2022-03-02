/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yun Ze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "vm.hpp"

class Chunk;
struct Token;

void printChunk(const Chunk& chunk, const std::string& name);
size_t printInstruction(const Chunk& chunk, size_t index);
void printTokens(const std::vector<std::shared_ptr<Token>>& tokens);
void printStack(MemoryStack& memory);
