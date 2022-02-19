#include "debug.hpp"

#include <iomanip>
#include <iostream>

#include "chunk.hpp"
#include "object.hpp"
#include "token.hpp"

void printValueType(Value value) {
  switch (value.getType()) {
    case VAL_BOOL:
      std::cout << "VAL_BOOL";
      break;
    case VAL_NULL:
      std::cout << "VAL_NULL";
      break;
    case VAL_NUM:
      std::cout << "VAL_NUM";
      break;
    case VAL_OBJECT:
      switch (OBJECT_TYPE(value)) {
        case OBJECT_BOUND_METHOD:
          std::cout << "OBJECT_BOUND_METHOD";
          break;
        case OBJECT_CLASS:
          std::cout << "OBJECT_CLASS";
          break;
        case OBJECT_CLOSURE:
          std::cout << "OBJECT_CLOSURE";
          break;
        case OBJECT_FUNCTION:
          std::cout << "OBJECT_FUNCTION";
          break;
        case OBJECT_INSTANCE:
          std::cout << "OBJECT_INSTANCE";
          break;
        case OBJECT_NATIVE:
          std::cout << "OBJECT_FUNCTION";
          break;
        case OBJECT_STRING:
          std::cout << "OBJECT_STRING";
          break;
        case OBJECT_UPVALUE:
          std::cout << "OBJECT_UPVALUE";
          break;
        case OBJECT_LIST:
          std::cout << "OBJECT_LIST";
          break;
      }
      break;
  }
  std::cout << std::endl;
}

size_t simpleInstruction(std::string name, size_t index) {
  std::cout << name << std::endl;
  return index + 1;
}

size_t constantInstruction(std::string name, Chunk& chunk, size_t index) {
  uint8_t constantIndex = chunk.getBytecodeAt(index + 1).code;
  std::cout << name;
  // const Value& constant = chunk.getConstantAt(constantIndex);
  std::cout << " " << (int)constantIndex;
  // constant.printValue();
  // std::cout << name << " ";
  // printValueType(constant);
  std::cout << std::endl;
  return index + 2;
}

size_t jumpInstruction(std::string name, int sign, Chunk& chunk, size_t index) {
  uint8_t high = chunk.getBytecodeAt(index + 1).code;
  uint8_t lo = chunk.getBytecodeAt(index + 2).code;
  uint16_t jump = (uint16_t)((high << 8) | lo);
  std::cout << name << " " << index + 3 + sign * jump << std::endl;
  return index + 3;
}

size_t invokeInstruction(std::string name, Chunk& chunk, size_t index) {
  uint8_t constant = chunk.getBytecodeAt(index + 1).code;
  uint8_t argCount = chunk.getBytecodeAt(index + 2).code;
  std::cout << name << " " << constant << " " << argCount << std::endl;
  return index + 3;
}

size_t printInstruction(Chunk& chunk, size_t index) {
  std::cout << std::setfill('0') << std::setw(5) << index << " ";
  std::cout << std::setfill(' ') << std::setw(5)
            << chunk.getBytecodeAt(index).line << " ";

  uint8_t code = chunk.getBytecodeAt(index).code;
  switch (code) {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", index);
    case OP_CONSTANT:
      return constantInstruction("OP_CONSTANT", chunk, index);
    case OP_NEGATE:
      return simpleInstruction("OP_NEGATE", index);
    case OP_ADD:
      return simpleInstruction("OP_ADD", index);
    case OP_SUBSTRACT:
      return simpleInstruction("OP_SUBTRACT", index);
    case OP_MULTIPLY:
      return simpleInstruction("OP_MULTIPLY", index);
    case OP_DIVIDE:
      return simpleInstruction("OP_DIVIDE", index);
    case OP_NULL:
      return simpleInstruction("OP_NULL", index);
    case OP_TRUE:
      return simpleInstruction("OP_TRUE", index);
    case OP_FALSE:
      return simpleInstruction("OP_FALSE", index);
    case OP_NOT:
      return simpleInstruction("OP_NOT", index);
    case OP_EQUAL:
      return simpleInstruction("OP_EQUAL", index);
    case OP_GREATER:
      return simpleInstruction("OP_GREATER", index);
    case OP_LESS:
      return simpleInstruction("OP_LESS", index);
    case OP_PRINT:
      return simpleInstruction("OP_PRINT", index);
    case OP_POP:
      return simpleInstruction("OP_POP", index);
    case OP_SET_GLOBAL:
      return constantInstruction("OP_SET_GLOBAL", chunk, index);
    case OP_GET_GLOBAL:
      return constantInstruction("OP_GET_GLOBAL", chunk, index);
    case OP_SET_LOCAL:
      return constantInstruction("OP_SET_LOCAL", chunk, index);
    case OP_GET_LOCAL:
      return constantInstruction("OP_GET_LOCAL", chunk, index);
    case OP_JUMP:
      return jumpInstruction("OP_JUMP", 1, chunk, index);
    case OP_JUMP_IF_FALSE:
      return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, index);
    case OP_LOOP:
      return jumpInstruction("OP_LOOP", -1, chunk, index);
    case OP_CALL:
      return constantInstruction("OP_CALL", chunk, index);
    case OP_MODULO:
      return simpleInstruction("OP_MODULO", index);
    case OP_CLOSURE:
      return constantInstruction("OP_CLOSURE", chunk, index);
    case OP_GET_UPVALUE:
      return constantInstruction("OP_GET_UPVALUE", chunk, index);
    case OP_SET_UPVALUE:
      return constantInstruction("OP_SET_UPVALUE", chunk, index);
    case OP_CLOSE_UPVALUE:
      return simpleInstruction("OP_CLOSE_OPVALUE", index);
    case OP_CLASS:
      return constantInstruction("OP_CLASS", chunk, index);
    case OP_GET_PROPERTY:
      return constantInstruction("OP_GET_PROPERTY", chunk, index);
    case OP_SET_PROPERTY:
      return constantInstruction("OP_SET_PROPERTY", chunk, index);
    case OP_METHOD:
      return constantInstruction("OP_METHOD", chunk, index);
    case OP_INVOKE:
      return invokeInstruction("OP_INVOKE", chunk, index);
    case OP_INHERIT:
      return simpleInstruction("OP_INHERIT", index);
    case OP_GET_SUPER:
      return constantInstruction("OP_GET_SUPER", chunk, index);
    case OP_SUPER_INVOKE:
      return invokeInstruction("OP_SUPER_INVOKE", chunk, index);
    case OP_ARRAY:
      return constantInstruction("OP_ARRAY", chunk, index);
    case OP_ARRAY_SET:
      return simpleInstruction("OP_ARRAY_SET", index);
    case OP_ARRAY_GET:
      return simpleInstruction("OP_ARRAY_GET", index);
    case OP_DUPLICATE:
      return constantInstruction("OP_DUPLICATE", index);
    default: {
      std::cout << "Unknown opcode " << code << std::endl;
      return index + 1;
    }
  }
}

void printChunk(Chunk& chunk, const std::string& name) {
  std::cout << "== BYTECODE FOR " << name << " ==" << std::endl;

  for (size_t i = 0; i < chunk.getBytecodeSize();) {
    i = printInstruction(chunk, i);
  }

  std::cout << std::endl;

  std::cout << "== CONSTANTS FOR " << name << " ==" << std::endl;

  for (size_t i = 0; i < chunk.getConstantsSize(); ++i) {
    std::cout << "Index " << i << ": ";
    chunk.getConstantAt(i).printValue();
    std::cout << " ";
    printValueType(chunk.getConstantAt(i));
  }

  std::cout << std::endl;

  // for (size_t i = 0; i < chunk.getConstantsSize(); ++i) {
  //   const Value& value = chunk.getConstantAt(i);
  //   if (IS_FUNCTION(value)) {
  //     std::shared_ptr<ObjectFunction> function = AS_FUNCTION(value);
  //     printChunk(function->getChunk(), function->getName()->getString());
  //   }
  // }
}

void printTokens(const std::vector<Token>& tokens) {
  std::cout << std::endl;
  std::cout << "== TOKENS ==" << std::endl;
  for (const auto& token : tokens) {
    TokenType type = token.type;
    switch (type) {
      case TOKEN_LPAREN:
        std::cout << "LPAREN" << std::endl;
        break;
      case TOKEN_RPAREN:
        std::cout << "RPAREN" << std::endl;
        break;
      case TOKEN_LBRACE:
        std::cout << "LBRACE" << std::endl;
        break;
      case TOKEN_RBRACE:
        std::cout << "RBRACE" << std::endl;
        break;
      case TOKEN_LBRACK:
        std::cout << "LBRACK" << std::endl;
        break;
      case TOKEN_RBRACK:
        std::cout << "RBRACK" << std::endl;
        break;
      case TOKEN_BECOMES:
        std::cout << "BECOMES" << std::endl;
        break;
      case TOKEN_DOT:
        std::cout << "DOT" << std::endl;
        break;
      case TOKEN_MINUS:
        std::cout << "MINUS" << std::endl;
        break;
      case TOKEN_PLUS:
        std::cout << "PLUS" << std::endl;
        break;
      case TOKEN_COMMA:
        std::cout << "COMMA" << std::endl;
        break;
      case TOKEN_STAR:
        std::cout << "STAR" << std::endl;
        break;
      case TOKEN_SLASH:
        std::cout << "SLASH" << std::endl;
        break;
      case TOKEN_SEMI:
        std::cout << "SEMI" << std::endl;
        break;
      case TOKEN_LT:
        std::cout << "LT" << std::endl;
        break;
      case TOKEN_GT:
        std::cout << "GT" << std::endl;
        break;
      case TOKEN_LE:
        std::cout << "LE" << std::endl;
        break;
      case TOKEN_GE:
        std::cout << "GE" << std::endl;
        break;
      case TOKEN_ID:
        std::cout << "ID: " + token.lexeme << std::endl;
        break;
      case TOKEN_NUM:
        std::cout << "NUM: " + token.lexeme << std::endl;
        break;
      case TOKEN_STRING:
        std::cout << "STRING: " + token.lexeme << std::endl;
        break;
      case TOKEN_EQ:
        std::cout << "EQ" << std::endl;
        break;
      case TOKEN_AND:
        std::cout << "AND" << std::endl;
        break;
      case TOKEN_OR:
        std::cout << "OR" << std::endl;
        break;
      case TOKEN_NOT:
        std::cout << "NOT" << std::endl;
        break;
      case TOKEN_IF:
        std::cout << "IF" << std::endl;
        break;
      case TOKEN_ELSE:
        std::cout << "ELSE" << std::endl;
        break;
      case TOKEN_WHILE:
        std::cout << "WHILE" << std::endl;
        break;
      case TOKEN_RETURN:
        std::cout << "RETURN" << std::endl;
        break;
      case TOKEN_PRINT:
        std::cout << "PRINT" << std::endl;
        break;
      case TOKEN_ADDR:
        std::cout << "ADDR" << std::endl;
        break;
      case TOKEN_AT:
        std::cout << "AT" << std::endl;
        break;
      case TOKEN_TRUE:
        std::cout << "TRUE" << std::endl;
        break;
      case TOKEN_FALSE:
        std::cout << "FALSE" << std::endl;
        break;
      case TOKEN_NULL:
        std::cout << "NULL" << std::endl;
        break;
      case TOKEN_EOF:
        std::cout << "EOF" << std::endl;
        break;
      case TOKEN_FOR:
        std::cout << "FOR" << std::endl;
        break;
      case TOKEN_FROM:
        std::cout << "FROM" << std::endl;
        break;
      case TOKEN_TO:
        std::cout << "TO" << std::endl;
        break;
      case TOKEN_BY:
        std::cout << "BY" << std::endl;
        break;
      case TOKEN_FUNCTION:
        std::cout << "FUNCTION" << std::endl;
        break;
      case TOKEN_PERC:
        std::cout << "PERC" << std::endl;
        break;
      case TOKEN_CLASS:
        std::cout << "CLASS" << std::endl;
        break;
      case TOKEN_THIS:
        std::cout << "THIS" << std::endl;
        break;
      case TOKEN_INHERITS:
        std::cout << "INHERITS" << std::endl;
        break;
      case TOKEN_SUPER:
        std::cout << "SUPER" << std::endl;
        break;
      case TOKEN_PLUSBECOMES:
        std::cout << "PLUSEQ" << std::endl;
        break;
      case TOKEN_MINUSBECOMES:
        std::cout << "MINUSEQ" << std::endl;
        break;
      case TOKEN_STARBECOMES:
        std::cout << "STAREQ" << std::endl;
        break;
      case TOKEN_SLASHBECOMES:
        std::cout << "SLASHEQ" << std::endl;
        break;
    }
  }
  std::cout << std::endl;
}

void printStack(MemoryStack& memory) {
  std::cout << "== STACK ==" << std::endl;
  std::cout << "Stack size: " << memory.size() << std::endl;
  for (size_t i = 0; i < memory.size(); ++i) {
    std::cout << "Element " << i << ": ";
    Value value = memory.getValueAt(i);
    if (IS_BOOL(value)) {
      if (AS_BOOL(value))
        std::cout << "true";
      else
        std::cout << "false";
    } else if (IS_NULL(value)) {
      std::cout << "null";
    } else if (IS_NUM(value)) {
      std::cout << AS_NUM(value);
    } else if (IS_OBJECT(value)) {
      AS_OBJECT(value)->printObject();
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void printGlobals(
    std::unordered_map<std::shared_ptr<ObjectString>, Value, ObjectString::Hash,
                       ObjectString::Comparator>& globals) {
  for (auto& it : globals) {
    std::cout << "Variable name: " << it.first->getString();
    std::cout << std::endl;
    std::cout << "Value: ";
    it.second.printValue();
    std::cout << std::endl;
  }
}