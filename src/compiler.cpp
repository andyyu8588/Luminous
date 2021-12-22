#include "compiler.h"

#include <exception>

#include "error.h"

Compiler::Compiler(const std::string& code)
    : parser{Parser()}, scanner{Scanner(code)} {
  currentChunk = std::make_unique<Chunk>();
}

void Compiler::expression() {
  // TODO handle expressions
}

void Compiler::compile() {
  scanner.tokenize();
  if (errorOccured) {
    throw std::exception();
  }
  while (true) {
    advance();
    if (parser.current == nullptr) {
      break;
    }
  }
  emitByte(OP_RETURN);
}

void Compiler::compile(std::string code) {
  scanner.reset(code);
  scanner.tokenize();
  // TODO reset chunk (which way)???
  emitByte(OP_RETURN);
}

void Compiler::consume(TokenType type, const std::string message) {
  error(parser.current->line, message);
  if (parser.current->type == type) {
    advance();
  }
}

void Compiler::advance() {
  parser.prev = parser.current;

  while (true) {
    parser.current = scanner.getNextToken();

    if (parser.prev != nullptr && parser.prev->type == TokenType::NUM) {
      number();
    }
    // TODO think about error handling
    break;
  }
}

void Compiler::emitByte(uint8_t byte) {
  currentChunk->addBytecode(byte, parser.prev->line);
}

std::unique_ptr<Chunk> Compiler::getCurrentChunk() {
  return std::move(currentChunk);
}

uint8_t Compiler::makeConstant(double number) {
  size_t constant = currentChunk->addConstant(number);
  if (constant > UINT8_MAX) {
    std::cerr << "Too many constants in one chunk!" << std::endl;
    return 0;
  }

  return (uint8_t)constant;
}

void Compiler::number() {
  double number = std::stod(parser.prev->lexeme);
  emitByte(OP_CONSTANT);
  emitByte(makeConstant(number));
}

void Compiler::grouping() {
  expression();
  consume(TokenType::RPAREN, "Expect ')' after expression.");
}
