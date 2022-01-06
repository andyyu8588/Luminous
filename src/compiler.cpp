#include "compiler.h"

#include <exception>

#include "error.h"

#ifdef DEBUG
#include "debug.h"
#endif

Compiler::Compiler() : parser{Parser()}, scanner{Scanner()} {
  for (int tokenNum = TOKEN_LPAREN; tokenNum <= TOKEN_EOF; ++tokenNum) {
    TokenType curToken = (TokenType)tokenNum;
    switch (curToken) {
      case TOKEN_LPAREN:
        ruleMap[TOKEN_LPAREN] = {std::bind(&Compiler::grouping, this, _1),
                                 nullptr, PREC_NONE};
        break;
      case TOKEN_MINUS:
        ruleMap[TOKEN_MINUS] = {std::bind(&Compiler::unary, this, _1),
                                std::bind(&Compiler::binary, this, _1),
                                PREC_TERM};
        break;
      case TOKEN_PLUS:
        ruleMap[TOKEN_PLUS] = {nullptr, std::bind(&Compiler::binary, this, _1),
                               PREC_TERM};
        break;
      case TOKEN_SLASH:
        ruleMap[TOKEN_SLASH] = {nullptr, std::bind(&Compiler::binary, this, _1),
                                PREC_FACTOR};
        break;
      case TOKEN_STAR:
        ruleMap[TOKEN_STAR] = {nullptr, std::bind(&Compiler::binary, this, _1),
                               PREC_FACTOR};
        break;
      case TOKEN_LT:
        ruleMap[TOKEN_LT] = {nullptr, std::bind(&Compiler::binary, this, _1),
                             PREC_COMPARISON};
        break;
      case TOKEN_GT:
        ruleMap[TOKEN_GT] = {nullptr, std::bind(&Compiler::binary, this, _1),
                             PREC_COMPARISON};
        break;
      case TOKEN_LE:
        ruleMap[TOKEN_LE] = {nullptr, std::bind(&Compiler::binary, this, _1),
                             PREC_COMPARISON};
        break;
      case TOKEN_GE:
        ruleMap[TOKEN_GE] = {nullptr, std::bind(&Compiler::binary, this, _1),
                             PREC_COMPARISON};
        break;

      case TOKEN_NUM:
        ruleMap[TOKEN_NUM] = {std::bind(&Compiler::number, this, _1), nullptr,
                              PREC_NONE};
        break;

      case TOKEN_EQ:
        ruleMap[TOKEN_EQ] = {nullptr, std::bind(&Compiler::binary, this, _1),
                             PREC_EQUALITY};
        break;

      case TOKEN_NOT:
        ruleMap[TOKEN_NOT] = {std::bind(&Compiler::unary, this, _1), nullptr,
                              PREC_NONE};
        break;

      case TOKEN_TRUE:
        ruleMap[TOKEN_TRUE] = {std::bind(&Compiler::literal, this, _1), nullptr,
                               PREC_NONE};
        break;
      case TOKEN_FALSE:
        ruleMap[TOKEN_FALSE] = {std::bind(&Compiler::literal, this, _1),
                                nullptr, PREC_NONE};
        break;
      case TOKEN_NULL:
        ruleMap[TOKEN_NULL] = {std::bind(&Compiler::literal, this, _1), nullptr,
                               PREC_NONE};
        break;
      case TOKEN_STRING:
        ruleMap[TOKEN_STRING] = {std::bind(&Compiler::string, this, _1),
                                 nullptr, PREC_NONE};
        break;

      case TOKEN_ID:
        ruleMap[TOKEN_ID] = {std::bind(&Compiler::variable, this, _1), nullptr,
                             PREC_NONE};
        break;
      default:
        ruleMap[curToken] = {nullptr, nullptr, PREC_NONE};
    }
  }
}

void Compiler::expression() { parsePrecendence(PREC_ASSIGNMENT); }

void Compiler::compile(const std::string& code) {
  // reset scope information:
  scopeDepth = 0;

  // init new chunk
  currentChunk = std::make_unique<Chunk>();

  // init scanner and tokenize
  scanner.reset(code);
  scanner.tokenize();
  if (errorOccured) {
    throw CompilerException();
  }

  // advance by 1 to get current and then parse
  advance();
  while (!match(TOKEN_EOF)) {
    declaration();
  }

  // end compiling
  if (errorOccured) {
    throw CompilerException();
  }
  emitByte(OP_RETURN);
}

void Compiler::consume(TokenType type, const std::string& message) {
  if (parser.current->type == type) {
    advance();
    return;
  }

  error(parser.current->line, message);
}

void Compiler::advance() {
  const Token& nextToken = scanner.getNextToken();
  parser.prev = parser.current;
  parser.current = &nextToken;
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
    error(parser.prev->line, "Expect expression. Found none.");
    return;
  }

  bool canAssign = (precedence <= PREC_ASSIGNMENT);
  prefixRule(canAssign);

  while (precedence <= getRule(parser.current->type)->precedence) {
    advance();
    ParseFunction infixRule = getRule(parser.prev->type)->infix;
    infixRule(canAssign);
  }

  if (canAssign && match(TOKEN_BECOMES)) {
    error(parser.current->line, "Invalid assignment target.");
  }
}

uint8_t Compiler::makeConstant(Value value) {
  size_t constant = currentChunk->addConstant(value);
  if (constant > UINT8_MAX) {
    std::cerr << "Too many constants in one chunk!" << std::endl;
    return 0;
  }

  return (uint8_t)constant;
}

void Compiler::number(bool canAssign) {
  (void)canAssign;
  double number = std::stod(parser.prev->lexeme);
  emitByte(OP_CONSTANT);
  emitByte(makeConstant(NUM_VAL(number)));
}

void Compiler::grouping(bool canAssign) {
  (void)canAssign;
  expression();
  consume(TOKEN_RPAREN, "Expect ')' after expression.");
}

void Compiler::unary(bool canAssign) {
  (void)canAssign;
  TokenType operatorType = parser.prev->type;

  parsePrecendence(PREC_UNARY);

  switch (operatorType) {
    case TOKEN_NOT:
      emitByte(OP_NOT);
      break;
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      return;  // unreachable
  }
}

void Compiler::binary(bool canAssign) {
  // TODO canAssign
  (void)canAssign;
  TokenType operatorType = parser.prev->type;

  ParseRule* rule = getRule(operatorType);
  parsePrecendence((Precedence)(rule->precedence + 1));

  switch (operatorType) {
    // comparison
    case TOKEN_GT:
      emitByte(OP_GREATER);
      break;
    case TOKEN_GE:
      emitByte(OP_LESS);
      emitByte(OP_NOT);
      break;
    case TOKEN_LT:
      emitByte(OP_LESS);
      break;
    case TOKEN_LE:
      emitByte(OP_GREATER);
      emitByte(OP_NOT);
      break;
    case TOKEN_EQ:
      emitByte(OP_EQUAL);
      break;

    // arithmetic
    case TOKEN_PLUS:
      emitByte(OP_ADD);
      break;
    case TOKEN_MINUS:
      emitByte(OP_SUBSTRACT);
      break;
    case TOKEN_STAR:
      emitByte(OP_MULTIPLY);
      break;
    case TOKEN_SLASH:
      emitByte(OP_DIVIDE);
      break;
    default:
      return;  // unreachable
  }
}

void Compiler::literal(bool canAssign) {
  (void)canAssign;
  TokenType operatorType = parser.prev->type;

  switch (operatorType) {
    case TOKEN_TRUE:
      emitByte(OP_TRUE);
      break;
    case TOKEN_FALSE:
      emitByte(OP_FALSE);
      break;
    case TOKEN_NULL:
      emitByte(OP_NULL);
      break;
    default:
      return;  // unreachable
  }
}

ParseRule* Compiler::getRule(TokenType type) { return &ruleMap[type]; }

void Compiler::string(bool canAssign) {
  (void)canAssign;
  emitByte(OP_CONSTANT);
  emitByte(makeConstant(
      OBJECT_VAL(std::make_shared<ObjectString>(parser.prev->lexeme))));
}

void Compiler::declaration() {
  // if (parser.current->type == TOKEN_ID) {
  //   varDeclaration();
  // } else {
  //   statement();
  // }

  statement();

  if (panicMode) synchronize();
}

void Compiler::beginScope() { scopeDepth++; }

void Compiler::endScope() {
  scopeDepth--;

  while (localVars.list.size() > 0 &&
         localVars.list.back()->depth > scopeDepth) {
    localVars.hash.erase(localVars.list.back());
    localVars.list.pop_back();
    emitByte(OP_POP);
  }
}

void Compiler::statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else if (match(TOKEN_LBRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}

void Compiler::block() {
  while (!(parser.current->type == TOKEN_RBRACE) &&
         !(parser.current->type == TOKEN_EOF)) {
    declaration();
  }

  consume(TOKEN_RBRACE, "Expect '}' after block.");
}

void Compiler::printStatement() {
  consume(TOKEN_LPAREN, "Expect '(' after 'print'.");
  expression();
  consume(TOKEN_RPAREN, "Expect ')' after expression.");
  consume(TOKEN_SEMI, "Expect ';' after statement.");
  emitByte(OP_PRINT);
}

void Compiler::expressionStatement() {
  bool pop = true;
  if (scopeDepth != 0) {
    pop = false;
    std::shared_ptr<Local> toCheck =
        std::make_shared<Local>(*(parser.current), scopeDepth);
    if (localVars.hash.contains(toCheck)) {
      pop = true;
    }
  }
  expression();
  consume(TOKEN_SEMI, "Expect ';' after statement.");
  if (pop) {
    emitByte(OP_POP);
  }
}

bool Compiler::match(TokenType type) {
  if (!(parser.current->type == type)) return false;
  advance();
  return true;
}

void Compiler::synchronize() {
  panicMode = false;

  while (parser.current->type != TOKEN_EOF) {
    if (parser.prev->type == TOKEN_SEMI) return;

    switch (parser.current->type) {
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_ADDR:
      case TOKEN_AT:
      case TOKEN_RETURN:
      case TOKEN_ID:
        return;
      default:;
    }

    advance();
  }
}

// void Compiler::varDeclaration() {
//   uint8_t global = parseVariable("Expect variable name.");

//   if (match(TOKEN_BECOMES)) {
//     expression();
//   } else {
//     emitByte(OP_NULL);
//   }
//   consume(TOKEN_SEMI, "Expect ';' after expression.");

//   emitByte(OP_DEFINE_GLOBAL);
//   emitByte(global);
// }

// uint8_t Compiler::parseVariable(std::string message) {
//   consume(TOKEN_ID, message);
//   return identifierConstant(parser.prev);
// }

uint8_t Compiler::identifierConstant(const Token* var) {
  std::shared_ptr<ObjectString> ptr =
      std::make_shared<ObjectString>(var->lexeme);
  auto it = existingStrings.find(ptr);
  if (it == existingStrings.end()) {
    existingStrings.insert(ptr);
    return makeConstant(OBJECT_VAL(ptr));
  }
  return makeConstant(OBJECT_VAL(*it));
}

void Compiler::markInitialized() { localVars.list.back()->depth = scopeDepth; }

void Compiler::declareLocal() {
  if (scopeDepth == 0) return;  // no need to add to localVars if global

  std::shared_ptr<Local> local =
      std::make_shared<Local>(*(parser.prev), scopeDepth);

  if (localVars.hash.contains(local)) return;  // variable exists already

  local->depth = -1;
  localVars.hash.insert(local);
  localVars.list.push_back(local);
}

void Compiler::variable(bool canAssign) {
  if (scopeDepth > 0 && canAssign && parser.current->type == TOKEN_BECOMES) {
    declareLocal();
  }
  namedVariable(parser.prev, canAssign);
}

// find a local's index in localVars with a token
int Compiler::resolveLocal(const Token* name) {
  for (int i = (int)(localVars.list.size()) - 1; i >= 0; i--) {
    std::shared_ptr<Local> curLocal = localVars.list.at(i);
    if (curLocal->name.lexeme == name->lexeme) {
      return i;
    }
  }

  return -1;
}

void Compiler::namedVariable(const Token* name, bool canAssign) {
  uint8_t getOp, setOp;
  int arg = resolveLocal(name);

  if (arg != -1) {
    getOp = OP_GET_LOCAL;
    setOp = OP_SET_LOCAL;
  } else {
    arg = identifierConstant(name);
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }

  if (canAssign && match(TOKEN_BECOMES)) {
    expression();
    if (localVars.list.size() > 0 && localVars.list.back()->depth == -1) {
      markInitialized();
    }
    emitByte(setOp);
  } else {
    if (getOp == OP_GET_LOCAL && localVars.list.at(arg)->depth == -1) {
      error(name->line, "Can't read local variable in its own initializer.");
    }
    emitByte(getOp);
  }
  emitByte((uint8_t)arg);
}

Local::Local(const Token& name, int depth) : name{name}, depth{depth} {}

size_t Local::Hash::operator()(const std::shared_ptr<Local>& local) const {
  return std::hash<std::string>{}(local->name.lexeme);
}

bool Local::Comparator::operator()(const std::shared_ptr<Local>& a,
                                   const std::shared_ptr<Local>& b) const {
  return a->depth == b->depth;
}
