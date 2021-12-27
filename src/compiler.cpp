#include "compiler.h"

#include <exception>

#include "error.h"

#ifdef DEBUG
#include "debug.h"
#endif

Compiler::Compiler() : parser{Parser()}, scanner{Scanner()} {}

void Compiler::expression() {
  panicMode = false;
  parsePrecendence(PREC_ASSIGNMENT);
}

void Compiler::compile(const std::string& code) {
  // init new chunk
  currentChunk = std::make_unique<Chunk>();

  // init scanner and tokenize
  scanner.reset(code);
  scanner.tokenize();
  if (errorOccured) {
    throw std::exception();
  }

  // advance by 1 to get current and then parse
  advance();
  expression();
  emitByte(OP_RETURN);
  if (errorOccured) {
    throw std::exception();
  }
}

void Compiler::consume(TokenType type, const std::string& message) {
  if (parser.current != nullptr && parser.current->type == type) {
    advance();
  } else if (!panicMode) {
    if (parser.current != nullptr) {
      error(parser.current->line, message);
    } else {
      error(parser.prev->line, message);
    }
  }
}

void Compiler::advance() {
  std::shared_ptr<Token> nextToken = scanner.getNextToken();
  if (parser.current == nullptr && nextToken == nullptr) {
    error(parser.prev->line, "Expect expression. Found none.");
  }
  parser.prev = parser.current;
  parser.current = nextToken;
}

void Compiler::emitByte(uint8_t byte) {
  currentChunk->addBytecode(byte, parser.prev->line);
#ifdef DEBUG
  if (byte == OP_RETURN) printChunk(*currentChunk);
#endif
}

std::unique_ptr<Chunk> Compiler::getCurrentChunk() {
  return std::move(currentChunk);
}

void Compiler::parsePrecendence(Precedence precedence) {
  advance();
  ParseFunction prefixRule = getRule(parser.prev->type)->prefix;
  if (prefixRule == nullptr) {
    std::cerr << "DEV: Expect expression. Found none in ruleMap." << std::endl;
    return;
  }
  prefixRule();

  while (parser.current != nullptr &&
         precedence <= getRule(parser.current->type)->precedence) {
    advance();
    ParseFunction infixRule = getRule(parser.prev->type)->infix;
    infixRule();
  }
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
  consume(TOKEN_RPAREN, "Expect ')' after expression.");
}

void Compiler::unary() {
  TokenType operatorType = parser.prev->type;

  parsePrecendence(PREC_UNARY);

  switch (operatorType) {
    case TOKEN_MINUS: {
      emitByte(OP_NEGATE);
      break;
    }
    default:
      return;
  }
}

void Compiler::binary() {
  TokenType operatorType = parser.prev->type;

  ParseRule* rule = getRule(operatorType);
  parsePrecendence((Precedence)(rule->precedence + 1));

  switch (operatorType) {
    case TOKEN_PLUS: {
      emitByte(OP_ADD);
      break;
    }
    case TOKEN_MINUS: {
      emitByte(OP_SUBSTRACT);
      break;
    }
    case TOKEN_STAR: {
      emitByte(OP_MULTIPLY);
      break;
    }
    case TOKEN_SLASH: {
      emitByte(OP_DIVIDE);
      break;
    }
    default:
      return;
  }
}

ParseRule* Compiler::getRule(TokenType type) { return &ruleMap[type]; }
