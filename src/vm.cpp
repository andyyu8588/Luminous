#include "vm.h"

#include <string>

#include "chunk.h"

#ifdef DEBUG
#include "debug.h"
#endif

void VM::binaryOperation(char operation) {
  double a = AS_NUM(memory.top());
  memory.pop();
  double b = AS_NUM(memory.top());
  memory.pop();
  switch (operation) {
    case '+': {
      memory.push(NUM_VAL(b + a));
      break;
    }
    case '-': {
      memory.push(NUM_VAL(b - a));
      break;
    }
    case '*': {
      memory.push(NUM_VAL(b * a));
      break;
    }
    case '/': {
      memory.push(NUM_VAL(b / a));
      break;
    }
  }
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
      case OP_ADD: {
        binaryOperation('+');
        break;
      }
      case OP_SUBSTRACT: {
        binaryOperation('-');
        break;
      }
      case OP_MULTIPLY: {
        binaryOperation('*');
        break;
      }
      case OP_DIVIDE: {
        binaryOperation('/');
        break;
      }
      case OP_NEGATE: {
        if (!IS_NUM(memory.top())) {
          // TODO: runtimeError printer
          return INTERPRET_RUNTIME_ERROR;
        }
        double a = AS_NUM(memory.top());
        memory.pop();
        memory.push(NUM_VAL(-a));
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
