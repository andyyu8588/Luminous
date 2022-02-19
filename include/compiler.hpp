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
  bool isCaptured = false;

  Local(const Token& name, int depth);

  struct Hash {
    size_t operator()(const std::shared_ptr<Local>& local) const;
  };

  struct Comparator {
    bool operator()(const std::shared_ptr<Local>&,
                    const std::shared_ptr<Local>&) const;
  };
};

struct Upvalue {
  const uint8_t index;
  const bool isLocal;

  bool operator==(const Upvalue&) const;
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

enum FunctionType { TYPE_FUNCTION, TYPE_METHOD, TYPE_SCRIPT, TYPE_CONSTRUCTOR };

struct FunctionInfo {
  std::shared_ptr<ObjectFunction> const function;
  const FunctionType type;
  std::vector<Upvalue> upvalues;

  FunctionInfo(std::shared_ptr<ObjectFunction> function, FunctionType type);
};

struct ClassInfo {
  const Token* name;
  bool hasSuperclass = false;

  ClassInfo(const Token* name);
};

class Compiler {
  Parser parser;
  Scanner scanner;
  std::unordered_map<TokenType, ParseRule> ruleMap;

  // for variables:
  GlobalVariables globalVars;
  std::vector<LocalVariables> localVars;
  int scopeDepth = 0;

  // for functions:
  std::vector<FunctionInfo> functions;

  // for classes:
  std::vector<ClassInfo> classes;

  // parser token management:
  // advance to the next token in the stream
  void advance();
  // advance with type checking
  void consume(TokenType type, const std::string& message);
  // returns true if the current token matches the given type and advances
  bool match(TokenType type);

  // returns the associated OpCode if the current token matches any, otherwise
  // returns 0
  OpCode matchBinaryEq();

  // returns the current chunk:
  Chunk& currentChunk();

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
  void call(bool canAssign);
  void dot(bool canAssign);
  void this_(bool canAssign);
  void super_(bool canAssign);
  void array(bool canAssign);
  void index(bool canAssign);

  ParseRule* getRule(TokenType type);

  void declaration();
  void statement();

  void printStatement();
  void expressionStatement();

  // variable assignment and retrieval:
  uint8_t identifierConstant(const Token* var);
  void namedVariable(const Token* name, bool canAssign);

  // local variables:
  void beginScope();
  void endScope();
  void block();
  void declareLocal();
  int resolveLocal(const Token* name, size_t index);
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
  uint8_t argumentList();
  void returnStatement();

  // closures:
  int resolveUpvalue(const Token*, size_t);
  int addUpvalue(uint8_t, bool, size_t);

  // classes:
  void classDeclaration();
  void method();
  std::shared_ptr<Token> syntheticToken(const std::string lexeme);

  // for error synchronization:
  void synchronize();

 public:
  void compile(const std::string& code, std::string currentFile);
  std::shared_ptr<ObjectFunction> getFunction();

  Compiler();
};
