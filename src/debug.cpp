#include "debug.h"

#include <iomanip>
#include <iostream>

#include "chunk.h"
#include "token.h"

size_t simpleInstruction(std::string name, size_t index) {
  std::cout << name << std::endl;
  return index + 1;
}

size_t constantInstruction(std::string name, Chunk& chunk, size_t index) {
  uint8_t constantIndex = chunk.getBytecodeAt(index).code;
  std::cout << name << " " << chunk.getConstantAt(constantIndex) << std::endl;
  return index + 2;
}

void printChunk(Chunk& chunk) {
  std::cout << "== BYTECODE ==" << std::endl;
  std::cout << std::endl;

  (void)chunk;
  // for (size_t index = 0; index < chunk.getBytecodeSize();) {
  //   index = printInstruction(chunk, index);
  // }
}

size_t printInstruction(Chunk& chunk, size_t index) {
  std::cout << std::setfill('0') << std::setw(5) << index << " ";
  std::cout << std::setfill(' ') << std::setw(5)
            << chunk.getBytecodeAt(index).line << " ";
  uint8_t instruction = chunk.getBytecodeAt(index).code;
  switch (instruction) {
    case OP_CONSTANT:
      return constantInstruction("CONSTANT", chunk, index);
    case OP_RETURN:
      return simpleInstruction("RETURN", index);
    default:
      std::cout << "Unkown opcode " << instruction << std::endl;
      return index + 1;
  }
}

void printTokens(const std::vector<std::shared_ptr<Token>>& tokens) {
  std::cout << "== TOKENS ==" << std::endl;
  for (const auto& token : tokens) {
    TokenType type = token->type;
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
        std::cout << "ID: " + token->lexeme << std::endl;
        break;
      case TOKEN_NUM:
        std::cout << "NUM: " + token->lexeme << std::endl;
        break;
      case TOKEN_STRING:
        std::cout << "STRING: " + token->lexeme << std::endl;
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
    }
  }
  std::cout << std::endl;
}

void printStack(std::stack<double>& memory) {
  std::cout << "== STACK ==" << std::endl;
  std::cout << "Stack size: " << memory.size() << std::endl;
  int counter = 0;
  while (memory.size() != 0) {
    std::cout << "Element " << counter << ": " << memory.top() << std::endl;
    memory.pop();
    ++counter;
  }
  std::cout << std::endl;
}
