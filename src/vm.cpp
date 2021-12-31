#include "vm.h"

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>

#include "chunk.h"
#include "object.h"

#ifdef DEBUG
#include "debug.h"
#endif

InterpretResult VM::binaryOperation(char operation) {
  Value a = memory.top();
  memory.pop();
  Value b = memory.top();
  memory.pop();

  if (IS_NUM(a) && IS_NUM(b)) {
    double c = AS_NUM(a);
    double d = AS_NUM(b);

    switch (operation) {
      case '+':
        memory.push(NUM_VAL(d + c));
        break;
      case '-':
        memory.push(NUM_VAL(d - c));
        break;
      case '*':
        memory.push(NUM_VAL(d * c));
        break;
      case '/':
        memory.push(NUM_VAL(d / c));
        break;
      case '>':
        memory.push(BOOL_VAL(d > c));
        break;
      case '<':
        memory.push(BOOL_VAL(d < c));
        break;
      default:
        return INTERPRET_RUNTIME_ERROR;  // unreachable
    }
  } else if (IS_STRING(a) && IS_STRING(b)) {
    const std::string& c = AS_STRING(a);
    const std::string& d = AS_STRING(b);

    switch (operation) {
      case '+':
        concatenate(c, d);
        break;
      default:
        std::string message = "Invalid operation '";
        message.push_back(operation);
        message += "' on strings.";
        runtimeError(message.c_str());
        return INTERPRET_RUNTIME_ERROR;
    }
  } else {
    runtimeError("Operands must be numbers or strings.");
    return INTERPRET_RUNTIME_ERROR;
  }
  return INTERPRET_OK;
}

void VM::resetMemory() { memory = std::stack<Value>(); }

void VM::runtimeError(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  unsigned int line = chunk->getPrevBytecode().line;
  std::cerr << "[line " << line << "] in code.\n" << std::endl;

  resetMemory();
}

InterpretResult VM::interpret(std::unique_ptr<Chunk> chunk) {
  this->chunk = std::move(chunk);
  return run();
}

InterpretResult VM::run() {
  while (true) {
    ByteCode bytecode = chunk->getBytecodeAtPC();
    switch (bytecode.code) {
      case OP_CONSTANT: {
        Value constant = chunk->getConstantAt(chunk->getBytecodeAtPC().code);
        memory.push(constant);
        break;
      }
      case OP_NULL: {
        memory.push(NULL_VAL);
        break;
      }
      case OP_TRUE: {
        memory.push(BOOL_VAL(true));
        break;
      }
      case OP_FALSE: {
        memory.push(BOOL_VAL(false));
        break;
      }
      case OP_POP: {
        memory.pop();
        break;
      }
      case OP_EQUAL: {
        Value a = memory.top();
        memory.pop();
        Value b = memory.top();
        memory.pop();
        memory.push(BOOL_VAL(a == b));
        break;
      }
      case OP_GREATER: {
        if (binaryOperation('>') == INTERPRET_RUNTIME_ERROR)
          return INTERPRET_RUNTIME_ERROR;
        break;
      }
      case OP_LESS: {
        if (binaryOperation('<') == INTERPRET_RUNTIME_ERROR)
          return INTERPRET_RUNTIME_ERROR;
        break;
      }
      case OP_ADD: {
        if (binaryOperation('+') == INTERPRET_RUNTIME_ERROR)
          return INTERPRET_RUNTIME_ERROR;
        break;
      }
      case OP_SUBSTRACT: {
        if (binaryOperation('-') == INTERPRET_RUNTIME_ERROR)
          return INTERPRET_RUNTIME_ERROR;
        break;
      }
      case OP_MULTIPLY: {
        if (binaryOperation('*') == INTERPRET_RUNTIME_ERROR)
          return INTERPRET_RUNTIME_ERROR;
        break;
      }
      case OP_DIVIDE: {
        if (binaryOperation('/') == INTERPRET_RUNTIME_ERROR)
          return INTERPRET_RUNTIME_ERROR;
        break;
      }
      case OP_NOT: {
        Value a = memory.top();
        memory.pop();
        memory.push(BOOL_VAL(isFalsey(a)));
        break;
      }
      case OP_NEGATE: {
        if (!IS_NUM(memory.top())) {
          runtimeError("Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        double a = AS_NUM(memory.top());
        memory.pop();
        memory.push(NUM_VAL(-a));
        break;
      }
      case OP_PRINT: {
        Value a = memory.top();
        memory.pop();
        a.printValue();
        std::cout << std::endl;
        break;
      }
      case OP_RETURN: {
#ifdef DEBUG
        printStack(memory);
#endif
        return INTERPRET_OK;
      }
    }
  }
  return INTERPRET_OK;
}

bool VM::isFalsey(Value value) const {
  return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value)) ||
         (IS_NUM(value) && !AS_NUM(value));
}

void VM::concatenate(const std::string& c, const std::string& d) {
  memory.push(OBJECT_VAL(std::make_shared<ObjectString>(c + d)));
}
