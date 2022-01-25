#pragma once
#include <functional>
#include <stack>
#include <unordered_set>

#include "chunk.hpp"
#include "object.hpp"
#include "scanner.hpp"

using namespace std::placeholders;
using ParseFunction = std::function<void(bool)>;

extern bool errorOccured;

class Compiler;

class CompilerException {};

enum Precedence {
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,
  PREC_COMPARISON,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_CALL,
  PREC_PRIMARY,
};

struct Parser {
  const Token* current;
  const Token* prev;
};

struct ParseRule {
  ParseFunction prefix;
  ParseFunction infix;
  Precedence precedence;
};

struct Local {
  const Token& name;
  int depth;

  Local(const Token& name, int depth);

  struct Hash {
    size_t operator()(const std::shared_ptr<Local>& local) const;
  };

  struct Comparator {
    bool operator()(const std::shared_ptr<Local>& a,
                    const std::shared_ptr<Local>& b) const;
  };
};

struct GlobalVariables {
  std::unordered_set<std::shared_ptr<ObjectString>, ObjectString::Hash,
                     ObjectString::Comparator>
      existingStrings;
  std::unordered_set<std::shared_ptr<ObjectString>, ObjectString::Hash,
                     ObjectString::Comparator>
      tempStrings;

  bool contains(const std::string&) const;
  std::shared_ptr<ObjectString> find(std::shared_ptr<ObjectString>);
  void migrate();
  void tempClear();
};

class LocalVariables {
  std::vector<std::shared_ptr<Local>> list;
  std::unordered_set<std::shared_ptr<Local>, Local::Hash, Local::Comparator>
      hash;

 public:
  std::shared_ptr<Local> at(size_t index);
  std::shared_ptr<Local> back();
  void clear();
  bool contains(const std::shared_ptr<Local>) const;
  void insert(const std::shared_ptr<Local>);
  void pop_back();
  size_t size() const;
};

enum FunctionType { TYPE_FUNCTION, TYPE_SCRIPT };

class Compiler {
  Parser parser;
  Scanner scanner;
  std::unordered_map<TokenType, ParseRule> ruleMap;

  // for variables:
  GlobalVariables globalVars;
  LocalVariables localVars;
  int scopeDepth = 0;

  // for functions:
  std::stack<std::shared_ptr<ObjectFunction>> functions;
  std::stack<FunctionType> funcTypes;

  // returns the current chunk:
  Chunk& currentChunk();

  // advance to the next token in the stream
  void advance();

  // advance with type checking
  void consume(TokenType type, const std::string& message);

  // returns true if the current token matches the given type and advances
  bool match(TokenType type);

  // add given byte to the current chunk
  void emitByte(uint8_t byte);

  // expression parsing functions:
  void expression();

  void parsePrecedence(Precedence precedence);

  // making a constant opcode and pushes it to the Chunk
  uint8_t makeConstant(Value number);

  // parsing functions:
  void binary(bool canAssign);
  void grouping(bool canAssign);
  void literal(bool canAssign);
  void number(bool canAssign);
  void string(bool canAssign);
  void unary(bool canAssign);
  void variable(bool canAssign);
  void andOperation(bool canAssign);
  void orOperation(bool canAssign);

  ParseRule* getRule(TokenType type);

  void declaration();
  void statement();

  void printStatement();
  void expressionStatement();

  // for variable assignment and retrieval
  uint8_t identifierConstant(const Token* var);
  void namedVariable(const Token* name, bool canAssign);

  // for local variables:
  void beginScope();
  void endScope();
  void block();
  void declareLocal();
  int resolveLocal(const Token* name);
  void markInitialized();
  bool inLocalVars(const Token&);

  // control flows:
  void ifStatement();
  int emitJump(uint8_t);
  void patchJump(int index);
  void whileStatement();
  void emitLoop(int);
  void forStatement();

  // functions:
  void functionDeclaration();
  void function(FunctionType type);
  void call(bool canAssign);
  uint8_t argumentList();
  void returnStatement();

  // for error synchronization:
  void synchronize();

 public:
  void compile(const std::string& code);
  std::shared_ptr<ObjectFunction> getFunction();

  Compiler();
};
