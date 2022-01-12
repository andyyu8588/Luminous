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
      case TOKEN_AND:
        ruleMap[TOKEN_AND] = {
            nullptr, std::bind(&Compiler::andOperation, this, _1), PREC_AND};
        break;
      case TOKEN_OR:
        ruleMap[TOKEN_OR] = {
            nullptr, std::bind(&Compiler::orOperation, this, _1), PREC_OR};
        break;
      default:
        ruleMap[curToken] = {nullptr, nullptr, PREC_NONE};
    }
  }
}

void Compiler::expression() { parsePrecedence(PREC_ASSIGNMENT); }

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

void Compiler::parsePrecedence(Precedence precedence) {
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

  parsePrecedence(PREC_UNARY);

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
  parsePrecedence((Precedence)(rule->precedence + 1));

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
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
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
  bool pop = scopeDepth == 0 || inLocalVars(*(parser.current)) ||
             existingStrings.contains(
                 std::make_shared<ObjectString>(parser.current->lexeme));
  expression();
  consume(TOKEN_SEMI, "Expect ';' after statement.");
  if (pop) {
    emitByte(OP_POP);
  }
}

void Compiler::ifStatement() {
  consume(TOKEN_LPAREN, "Expect '(' after 'if' keyword.");
  expression();
  consume(TOKEN_RPAREN, "Expect ')' to close condition statement.");

  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();

  int elseJump = emitJump(OP_JUMP);

  patchJump(thenJump);
  emitByte(OP_POP);
  if (match(TOKEN_ELSE)) statement();
  patchJump(elseJump);
}

int Compiler::emitJump(uint8_t inst) {
  emitByte(inst);
  emitByte(0xff);
  emitByte(0xff);
  return currentChunk->getBytecodeSize() - 2;
}

void Compiler::whileStatement() {
  int loopStart = currentChunk->getBytecodeSize();
  consume(TOKEN_LPAREN, "Expect '(' after 'while' keyword.");
  expression();
  consume(TOKEN_RPAREN, "Expect ')' to close condition statement.");

  int exitJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  statement();

  emitLoop(loopStart);

  patchJump(exitJump);
  emitByte(OP_POP);
}

void Compiler::forStatement() {
  // each for loop is a new scope
  beginScope();

  consume(TOKEN_LPAREN, "Expect '(' after 'for' keyword.");
  consume(TOKEN_ID, "Expect a variable for loop initializer.");

  /* initializer clause */

  const Token* varName = parser.prev;
  bool inLocal = inLocalVars(*(varName));
  bool inGlobal =
      existingStrings.contains(std::make_shared<ObjectString>(varName->lexeme));

  // if it's a new variable, then we need to add it in localVars
  if (!inLocal && !inGlobal) {
    std::shared_ptr<Local> local = std::make_shared<Local>(*(varName), -1);
    localVars.hash.insert(local);
    localVars.list.push_back(local);
  }

  consume(TOKEN_FROM, "Expect 'from' delimiter in for loop declaration.");

  // variable is in localVars for sure now, so get the index and push expression
  // on stack
  int index = 0;
  if (!inGlobal) {
    index = resolveLocal(varName);
  } else {
    index = identifierConstant(varName);
  }
  expression();

  // mark initialized since we got the RHS from expression() already
  if (!inLocal && !inGlobal) {
    markInitialized();
  }

  // set the value on the stack, we already have the index
  if (!inGlobal) {
    emitByte(OP_SET_LOCAL);
  } else {
    emitByte(OP_SET_GLOBAL);
  }
  emitByte((uint8_t)index);

  // pop from the stack if it was already declared before
  if (inLocal || inGlobal) {
    emitByte(OP_POP);
  }

  /* condition expression */

  int loopStart = currentChunk->getBytecodeSize();

  consume(TOKEN_TO, "Expect 'to' delimiter in for loop declaration.");

  // get the value of variable and the expression it's being compared to
  if (!inGlobal) {
    emitByte(OP_GET_LOCAL);
  } else {
    emitByte(OP_GET_GLOBAL);
  }
  emitByte((uint8_t)index);
  expression();

  consume(TOKEN_BY,
          "Expect 'by' to define the incrementor in for loop declaration.");

  // check if the incrementer is negative
  bool negativeInc = match(TOKEN_MINUS);

  consume(TOKEN_NUM, "Expect a numerical incrementor in for loop declaration.");
  const Token* inc = parser.prev;
  double numInc = std::stod(inc->lexeme);

  // emit based on negative or not
  if (negativeInc) {
    emitByte(OP_GREATER);
  } else {
    emitByte(OP_LESS);
  }

  // Jump out of the loop if condition becomes false
  int exitJump = emitJump(OP_JUMP_IF_FALSE);
  // And pop the condition from the stack because we don't need it anymore
  emitByte(OP_POP);

  int bodyJump = emitJump(OP_JUMP);

  /* increment expression */

  int incrementStart = currentChunk->getBytecodeSize();

  if (!inGlobal) {
    emitByte(OP_GET_LOCAL);
  } else {
    emitByte(OP_GET_GLOBAL);
  }
  emitByte((uint8_t)index);
  emitByte(OP_CONSTANT);
  emitByte(makeConstant(NUM_VAL(numInc)));
  if (negativeInc) {
    emitByte(OP_NEGATE);
  }
  emitByte(OP_ADD);
  if (!inGlobal) {
    emitByte(OP_SET_LOCAL);
  } else {
    emitByte(OP_SET_GLOBAL);
  }
  emitByte((uint8_t)index);

  // don't need the expression after it is incremented anymore
  emitByte(OP_POP);
  consume(TOKEN_RPAREN, "Expect ')' after for clauses.");

  emitLoop(loopStart);
  loopStart = incrementStart;
  patchJump(bodyJump);

  // Loop body:
  statement();
  emitLoop(loopStart);

  patchJump(exitJump);
  emitByte(OP_POP);

  endScope();
}

void Compiler::emitLoop(int loopStart) {
  emitByte(OP_LOOP);

  int index = currentChunk->getBytecodeSize() - loopStart + 2;
  if (index > UINT16_MAX) error(parser.prev->line, "Loop body too large.");

  emitByte((index >> 8) & 0xff);
  emitByte(index & 0xff);
}

void Compiler::patchJump(int index) {
  int jump = currentChunk->getBytecodeSize() - index - 2;

  if (jump > UINT16_MAX) {
    error(parser.prev->line, "Too much code to jump over.");
  }
  uint8_t code1 = (jump >> 8) & 0xff;
  uint8_t code2 = jump & 0xff;
  currentChunk->modifyCodeAt(code1, index);
  currentChunk->modifyCodeAt(code2, index + 1);
}

void Compiler::andOperation(bool canAssign) {
  (void)canAssign;
  int endJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  parsePrecedence(PREC_AND);

  patchJump(endJump);
}

void Compiler::orOperation(bool canAssign) {
  (void)canAssign;
  int elseJump = emitJump(OP_JUMP_IF_FALSE);
  int endJump = emitJump(OP_JUMP);

  patchJump(elseJump);
  emitByte(OP_POP);

  parsePrecedence(PREC_OR);
  patchJump(endJump);
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

  // variable exists already:
  if (existingStrings.contains(
          std::make_shared<ObjectString>(parser.prev->lexeme)))
    return;
  if (localVars.hash.contains(local)) return;

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

  // if found in localVars set:
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
    // initialize new local var
    if (localVars.list.size() > 0 && localVars.list.back()->depth == -1) {
      markInitialized();
    }
    emitByte(setOp);
  } else {
    // if local var and not initialized (self-use initialization):
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
  return a->name.lexeme == b->name.lexeme;
}

bool Compiler::inLocalVars(const Token& token) {
  if (scopeDepth == 0) return false;

  std::shared_ptr<Local> toCheck = std::make_shared<Local>(token, scopeDepth);

  return localVars.hash.contains(toCheck);
}