#include "vm.h"

#include <string>

#include "chunk.h"

#ifdef DEBUG
#include <iostream>
#endif

void VM::binaryOperation(char operation) {
  double a = memory.top();
  memory.pop();
  double b = memory.top();
  memory.pop();
  switch (operation) {
    case '+': {
      memory.push(a + b);
      break;
    }
    case '-': {
      memory.push(a - b);
      break;
    }
    case '*': {
      memory.push(a * b);
      break;
    }
    case '/': {
      memory.push(a / b);
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
        double constant = chunk->getConstantAt(chunk->getBytecodeAtPC().code);
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
        double top = memory.top();
        memory.pop();
        memory.push(-top);
        break;
      }
      case OP_RETURN: {
#ifdef DEBUG
        std::cout << "Stack size: " << memory.size() << std::endl;
        int counter = 0;
        while (memory.size() != 0) {
          std::cout << "Element " << counter << ": " << memory.top()
                    << std::endl;
          memory.pop();
          ++counter;
        }
#endif
        return INTERPRET_OK;
      }
    }
  }
  return INTERPRET_OK;
}
