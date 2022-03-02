/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yun Ze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "value.hpp"

enum OpCode {
  OP_CONSTANT,
  OP_NULL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_GET_UPVALUE,
  OP_SET_UPVALUE,
  OP_GET_PROPERTY,
  OP_SET_PROPERTY,
  OP_GET_SUPER,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBSTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_MODULO,
  OP_NOT,
  OP_NEGATE,
  OP_PRINT,
  OP_JUMP,
  OP_JUMP_IF_FALSE,
  OP_LOOP,
  OP_CALL,
  OP_INVOKE,
  OP_SUPER_INVOKE,
  OP_CLOSURE,
  OP_CLOSE_UPVALUE,
  OP_RETURN,
  OP_METHOD,
  OP_CLASS,
  OP_INHERIT,
  OP_ARRAY,
  OP_ARRAY_SET,
  OP_ARRAY_GET,
  OP_DUPLICATE,
  OP_FIELD,
  OP_NOP
};

enum AccessModifier {
  ACCESS_PRIVATE,
  ACCESS_PROTECTED,
  ACCESS_PUBLIC,
};

struct ByteCode {
  uint8_t code;
  const unsigned int line;
  const std::string filename;

  ByteCode(uint8_t code, unsigned int line, const std::string& filename);
};

class Chunk {
 private:
  std::vector<ByteCode> bytecode;
  std::vector<Value> constants;

 public:
  // bytecode vector getters and setters:
  size_t getBytecodeSize() const;
  ByteCode getBytecodeAt(size_t index) const;
  void addBytecode(uint8_t byte, unsigned int line, std::string filename);
  void modifyCodeAt(uint8_t newCode, int index);

  // constants vector getters and setters:
  size_t getConstantsSize() const;
  Value getConstantAt(size_t index) const;
  size_t addConstant(Value value);  // returns the index in the vector
};
