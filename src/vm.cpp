#include "vm.hpp"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "chunk.hpp"
#include "object.hpp"

#ifdef DEBUG
#include "debug.hpp"
#endif

VM::VM() {
  defineNative("clock", std::bind(&VM::clockNative, this, std::placeholders::_1,
                                  std::placeholders::_2));
  defineNative("substring",
               std::bind(&VM::substringNative, this, std::placeholders::_1,
                         std::placeholders::_2));
  defineNative("size", std::bind(&VM::sizeNative, this, std::placeholders::_1,
                                 std::placeholders::_2));
}

Value MemoryStack::getValueAt(size_t index) const { return c[index]; }

Value* MemoryStack::getValuePtrAt(size_t index) const {
  return (Value*)&(c[index]);
}

void MemoryStack::setValueAt(Value value, size_t index) { c[index] = value; }

void VM::binaryOperation(char operation) {
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
      case '%':
        memory.push(NUM_VAL(fmod(d, c)));
        break;
      default:
        throw VMException();  // unreachable
    }
  } else if (IS_STRING(a) && IS_STRING(b)) {
    const std::string& c = AS_STRING(a);
    const std::string& d = AS_STRING(b);

    switch (operation) {
      case '+':
        concatenate(c, d);
        break;
      case '>':
        memory.push(BOOL_VAL(d > c));
        break;
      case '<':
        memory.push(BOOL_VAL(d < c));
        break;
      default:
        std::string symbol(1, operation);
        runtimeError("Invalid operation '%s' on strings.", symbol.c_str());
    }
  } else if (IS_STRING(b) && IS_NUM(a)) {
    const std::string& c = AS_STRING(b);
    double d = AS_NUM(a);
    switch (operation) {
      case '+':
        concatenate(c, d);
        break;
      default:
        std::string symbol(1, operation);
        runtimeError("Invalid operation '%s' on string and number.",
                     symbol.c_str());
    }
  } else if (IS_LIST(b)) {
    switch (operation) {
      case '+': {
        std::shared_ptr<ObjectList> curList = AS_OBJECTLIST(b);
        std::vector<Value> newList;
        for (unsigned i = 0; i < curList->size(); i++) {
          newList.push_back(curList->get(i));
        }
        newList.push_back(a);
        memory.push(OBJECT_VAL(std::make_shared<ObjectList>(newList)));
        break;
      }
      case '-': {
        if (!IS_NUM(a)) {
          runtimeError("Index must be a positive integer.");
        }
        double index = AS_NUM(a);
        if (index < 0) {
          runtimeError("Index must be a positive integer");
        }
        if (std::floor(index) != std::ceil(index)) {
          runtimeError("Index must be a positive integer");
        }
        std::shared_ptr<ObjectList> curList = AS_OBJECTLIST(b);
        if (index >= curList->size()) {
          runtimeError("Index out of bounds.");
        }
        std::vector<Value> newList;
        for (unsigned i = 0; i < curList->size(); i++) {
          newList.push_back(curList->get(i));
        }
        newList.erase(newList.begin() + (unsigned)index);
        memory.push(OBJECT_VAL(std::make_shared<ObjectList>(newList)));
        break;
      }
      case '*': {
        if (!IS_NUM(a)) {
          runtimeError("Multiplier must be a positive integer.");
        }
        double mult = AS_NUM(a);
        if (mult < 0) {
          runtimeError("Multiplier must be a positive integer");
        }
        if (std::floor(mult) != std::ceil(mult)) {
          runtimeError("Multiplier must be a positive integer");
        }
        std::shared_ptr<ObjectList> curList = AS_OBJECTLIST(b);
        std::vector<Value> newList;
        for (unsigned i = 0; i < curList->size(); i++) {
          newList.push_back(curList->get(i));
        }
        std::vector<Value> newDuplicatedList;
        for (unsigned i = 0; i < (unsigned)mult; i++) {
          newDuplicatedList.insert(newDuplicatedList.end(), newList.begin(),
                                   newList.end());
        }
        memory.push(
            OBJECT_VAL(std::make_shared<ObjectList>(newDuplicatedList)));
        break;
      }
      default: {
        std::string symbol(1, operation);
        runtimeError("Invalid operation '%s' on array and value type.",
                     symbol.c_str());
      }
    }
  } else {
    runtimeError("Invalid operands.");
  }
}

void VM::resetMemory() { memory = MemoryStack(); }

void VM::runtimeError(const char* format, ...) {
#ifdef DEBUG
  printStack(memory);
#endif
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  while (!frames.empty()) {
    CallFrame& frame = frames.top();
    ObjectFunction& function = *(frame.closure.getFunction());

    std::cerr << "[line "
              << function.getChunk().getBytecodeAt(frame.PC - 1).line
              << " in file "
              << function.getChunk().getBytecodeAt(frame.PC - 1).filename
              << "] in ";

    if (function.getName() == nullptr) {
      std::cerr << "script" << std::endl;
    } else {
      std::cerr << function.getName()->getString() << "()" << std::endl;
    }
    frames.pop();
  }

  std::cerr << "(Runtime Error)" << std::endl;
  resetMemory();
  throw VMException();
}

void VM::interpret(std::shared_ptr<ObjectFunction> function) {
  std::shared_ptr<ObjectClosure> closure =
      std::make_shared<ObjectClosure>(function);
  memory.push(OBJECT_VAL(closure));
  callValue(OBJECT_VAL(closure), 0);
  run();
}

void VM::run() {
  CallFrame* frame = &(frames.top());
  while (true) {
    switch (readByte()) {
      case OP_CONSTANT: {
        Value constant = getTopChunk().getConstantAt(readByte());
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
      case OP_GET_LOCAL: {
        uint8_t slot = readByte();
        memory.push(memory.getValueAt(slot + frame->stackPos));
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = readByte();
        memory.setValueAt(memory.top(), slot + frame->stackPos);
        break;
      }
      case OP_GET_GLOBAL: {
        Value constantName = readConstant();
        std::shared_ptr<ObjectString> name = AS_OBJECTSTRING(constantName);
        auto it = globals.find(name);
        if (it == globals.end()) {
          runtimeError("Undefined variable '%s'.", name->getString().c_str());
        }
        memory.push(it->second);
        break;
      }
      case OP_SET_GLOBAL: {
        Value constantName = readConstant();
        std::shared_ptr<ObjectString> name = AS_OBJECTSTRING(constantName);
        globals.insert_or_assign(name, memory.top());
        break;
      }
      case OP_GET_PROPERTY: {
        if (!IS_INSTANCE(memory.top())) {
          runtimeError("Only instances have properties.");
        }

        ObjectInstance& instance = *(AS_INSTANCE(memory.top()));
        std::shared_ptr<ObjectString> name = AS_OBJECTSTRING(readConstant());
        std::shared_ptr<ObjectString> className =
            AS_OBJECTSTRING(readConstant());
        const AccessModifier* am =
            instance.getInstanceOf().getAccessModifier(name);
        bool isMethod = instance.getInstanceOf().getMethod(name) != nullptr;
        if (!isMethod) {
          if (am == nullptr) {
            runtimeError(
                "Field %s is not declared in class %s.",
                name->getString().c_str(),
                instance.getInstanceOf().getName().getString().c_str());
          }

          if (instance.getInstanceOf().getName().getString() !=
                  className->getString() &&
              *(am) != AccessModifier::ACCESS_PUBLIC) {
            if (*(am) == AccessModifier::ACCESS_PROTECTED) {
              runtimeError(
                  "Cannot access a protected field outside of a non-inherited "
                  "class.");
            } else {
              runtimeError(
                  "Cannot access a private field outside of the owner class.");
            }
          }
        }

        const Value* value = instance.getField(name);
        if (value != nullptr) {
          memory.pop();
          memory.push(*value);
          break;
        }
        bindMethod(instance.getInstanceOf(), name);
        break;
      }
      case OP_SET_PROPERTY: {
        Value value = memory.top();
        memory.pop();

        if (!IS_INSTANCE(memory.top())) {
          runtimeError("Only instances have fields.");
        }

        ObjectInstance* instance = AS_INSTANCE(memory.top()).get();
        memory.pop();

        std::shared_ptr<ObjectString> name = AS_OBJECTSTRING(readConstant());
        std::shared_ptr<ObjectString> className =
            AS_OBJECTSTRING(readConstant());
        const AccessModifier* am =
            instance->getInstanceOf().getAccessModifier(name);
        if (am == nullptr) {
          runtimeError("Field %s is not declared in class %s.",
                       name->getString().c_str(),
                       instance->getInstanceOf().getName().getString().c_str());
        }

        if (instance->getInstanceOf().getName().getString() !=
                className->getString() &&
            *(am) != AccessModifier::ACCESS_PUBLIC) {
          if (*(am) == AccessModifier::ACCESS_PROTECTED) {
            runtimeError(
                "Cannot access a protected field outside of a non-inherited "
                "class.");
          } else {
            runtimeError(
                "Cannot access a private field outside of the owner class.");
          }
        }

        instance->setField(name, value);
        memory.push(value);
        break;
      }
      case OP_GET_SUPER: {
        std::shared_ptr<ObjectString> name = AS_OBJECTSTRING(readConstant());
        std::shared_ptr<ObjectClass> superclass = AS_CLASS(memory.top());
        memory.pop();
        bindMethod(*superclass, name);
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
        binaryOperation('>');
        break;
      }
      case OP_LESS: {
        binaryOperation('<');
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
      case OP_MODULO: {
        binaryOperation('%');
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
      case OP_JUMP: {
        frame->PC += readShort();
        break;
      }
      case OP_JUMP_IF_FALSE: {
        uint16_t offset = readShort();
        if (isFalsey(memory.top())) frame->PC += offset;
        break;
      }
      case OP_LOOP: {
        frame->PC -= readShort();
        break;
      }
      case OP_CALL: {
        const int argCount = readByte();
        const size_t argStart = memory.size() - 1 - argCount;
        callValue(memory.getValueAt(argStart), argCount);
        frame = &(frames.top());
        break;
      }
      case OP_INVOKE: {
        std::shared_ptr<ObjectString> method = AS_OBJECTSTRING(readConstant());
        int argCount = readByte();
        invoke(method, argCount);
        frame = &(frames.top());
        break;
      }
      case OP_SUPER_INVOKE: {
        std::shared_ptr<ObjectString> method = AS_OBJECTSTRING(readConstant());
        int argCount = readByte();
        std::shared_ptr<ObjectClass> superclass = AS_CLASS(memory.top());
        memory.pop();
        invokeFromClass(*superclass, method, argCount);
        frame = &(frames.top());
        break;
      }
      case OP_CLOSURE: {
        std::shared_ptr<ObjectFunction> function = AS_FUNCTION(readConstant());
        std::shared_ptr<ObjectClosure> closure =
            std::make_shared<ObjectClosure>(function);
        memory.push(OBJECT_VAL(closure));
        for (int i = 0; i < closure->getUpvalueCount(); i++) {
          uint8_t isLocal = readByte();
          uint8_t index = readByte();
          if (isLocal) {
            closure->addUpvalue(
                captureUpvalue(memory.getValuePtrAt(frame->stackPos + index),
                               frame->stackPos + index));
          } else {
            closure->addUpvalue(frame->closure.getUpvalue(index));
          }
        }
        break;
      }
      case OP_INHERIT: {
        Value parent = memory.getValueAt(memory.size() - 2);
        if (!IS_CLASS(parent)) {
          runtimeError("Must inherit from a class.");
        }
        std::shared_ptr<ObjectClass> child = AS_CLASS(memory.top());
        child->copyMethodsFrom(*(AS_CLASS(parent)));
        child->copyFieldsFrom(*(AS_CLASS(parent)));
        memory.pop();  // Pop the child class
        break;
      }
      case OP_GET_UPVALUE: {
        uint8_t slot = readByte();
        memory.push(*(frame->closure.getUpvalue(slot)->getLocation()));
        break;
      }
      case OP_SET_UPVALUE: {
        uint8_t slot = readByte();
        *(frame->closure.getUpvalue(slot)->getLocation()) = memory.top();
        break;
      }
      case OP_CLOSE_UPVALUE: {
        closeUpvalues(memory.size() - 1);
        memory.pop();
        break;
      }
      case OP_FIELD: {
        defineField(AS_OBJECTSTRING(readConstant()));
        break;
      }
      case OP_METHOD: {
        defineMethod(AS_OBJECTSTRING(readConstant()));
        break;
      }
      case OP_RETURN: {
        // retrieve and pop return value
        Value top = memory.top();
        memory.pop();
        closeUpvalues(frame->stackPos);

        // pop all local variables alongside function object
        if (frames.size() > 1) {
          size_t initialMemorySize = memory.size();
          for (size_t i = frames.top().stackPos; i < initialMemorySize; i++) {
            memory.pop();
          }
        } else {  // if we are at <script>, only pop once, since there shouldn't
                  // be any local variables
          memory.pop();
        }

        // pop function frame
        frames.pop();

        if (frames.empty()) {
#ifdef DEBUG
          if (memory.size() != 0) {
            std::cout << "PANIC: STACK IS NOT EMPTY!" << std::endl << std::endl;
          }
          printStack(memory);
#endif
          if (memory.size() != 0) {
            runtimeError("Stack is not empty.");
          }
          return;
        }

        // push return value on stack (for outer scope) and set next frame
        memory.push(top);
        frame = &(frames.top());
        break;
      }
      case OP_CLASS: {
        memory.push(OBJECT_VAL(
            std::make_shared<ObjectClass>(AS_STRING(readConstant()))));
        break;
      }
      case OP_ARRAY: {
        uint8_t itemNum = readByte();
        std::shared_ptr<ObjectList> arr = std::make_shared<ObjectList>();
        for (unsigned i = memory.size() - itemNum; i < memory.size(); i++) {
          arr->add(memory.getValueAt(i));
        }
        for (unsigned i = 0; i < itemNum; i++) {
          memory.pop();
        }
        memory.push(OBJECT_VAL(arr));
        break;
      }
      case OP_ARRAY_GET: {
        Value& index = memory.top();
        if (!IS_NUM(index)) {
          runtimeError("Index must be a positive integer.");
        }
        double indexVal = AS_NUM(index);
        if (indexVal < 0) {
          runtimeError("Index must be a positive integer.");
        }
        if (ceil(indexVal) != floor(indexVal)) {
          runtimeError("Index must be a positive integer.");
        }
        memory.pop();
        std::shared_ptr<ObjectList> arr = AS_OBJECTLIST(memory.top());
        if (indexVal >= arr->size()) {
          runtimeError("Index out of bounds.");
        }
        memory.pop();
        memory.push(arr->get(indexVal));
        break;
      }
      case OP_ARRAY_SET: {
        Value value = memory.top();
        memory.pop();
        Value& index = memory.top();
        if (!IS_NUM(index)) {
          runtimeError("Index must be a postive integer.");
        }
        double indexVal = AS_NUM(index);
        if (indexVal < 0) {
          runtimeError("Index must be a positive integer.");
        }
        if (ceil(indexVal) != floor(indexVal)) {
          runtimeError("Index must be a positive integer.");
        }
        memory.pop();
        std::shared_ptr<ObjectList> arr = AS_OBJECTLIST(memory.top());
        if (indexVal >= arr->size()) {
          runtimeError("Index out of bounds.");
        }
        arr->set(value, indexVal);
        break;
      }
      case OP_DUPLICATE: {
        uint8_t repeat = readByte();
        size_t initialMemorySize = memory.size();
        for (unsigned i = memory.size() - repeat; i < initialMemorySize; ++i) {
          memory.push(memory.getValueAt(i));
        }
        break;
      }
    }
  }
}

void VM::call(std::shared_ptr<ObjectClosure> closure, int argCount) {
  if (argCount != closure->getFunction()->getArity()) {
    runtimeError("Expected %d arguments but found %d.",
                 closure->getFunction()->getArity(), argCount);
  }

  if (frames.size() == FRAMES_MAX) {
    runtimeError("Stack overflow.");
  }

  CallFrame newFrame{*closure, memory.size() - argCount - 1, 0};
  frames.push(newFrame);
}

void VM::callValue(Value callee, int argCount) {
  if (IS_OBJECT(callee)) {
    switch (OBJECT_TYPE(callee)) {
      case OBJECT_BOUND_METHOD: {
        ObjectBoundMethod* bound = AS_BOUND_METHOD(callee).get();
        memory.setValueAt(bound->getReceiver(), memory.size() - 1 - argCount);
        call(bound->getMethod(), argCount);
        return;
      }
      case OBJECT_CLASS: {
        ObjectClass* instanceOf = AS_CLASS(callee).get();
        memory.setValueAt(
            OBJECT_VAL(std::make_shared<ObjectInstance>(*instanceOf)),
            memory.size() - 1 - argCount);
        const Value* initializer = instanceOf->getMethod(constructorString);
        if (initializer != nullptr) {
          return call(AS_CLOSURE(*initializer), argCount);
        } else if (argCount != 0) {
          runtimeError("Expected 0 arguments but got %d.", argCount);
        }
        return;
      }
      case OBJECT_CLOSURE:
        call(AS_CLOSURE(callee), argCount);
        return;
      case OBJECT_NATIVE: {
        NativeFn native = AS_NATIVE(callee)->getFunction();
        Value result = native(argCount, memory.size() - argCount);
        for (int i = 0; i <= argCount; i++) {
          memory.pop();
        }
        memory.push(result);
        return;
      }
      default:
        break;
    }
  }

  runtimeError("Can only call functions and classes.");
}

bool VM::isFalsey(Value value) const {
  return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value)) ||
         (IS_NUM(value) && !AS_NUM(value));
}

void VM::concatenate(const std::string& c, const std::string& d) {
  memory.push(OBJECT_VAL(std::make_shared<ObjectString>(d + c)));
}

void VM::concatenate(const std::string& c, double d) {
  std::stringstream num;
  num << d;
  std::string numStr;
  num >> numStr;
  memory.push(OBJECT_VAL(std::make_shared<ObjectString>(c + numStr)));
}

Chunk& VM::getTopChunk() {
  return frames.top().closure.getFunction()->getChunk();
}

uint8_t VM::readByte() {
  uint8_t byte = getTopChunk().getBytecodeAt(frames.top().PC).code;
  frames.top().PC++;
  return byte;
}

Value VM::readConstant() { return getTopChunk().getConstantAt(readByte()); }

uint16_t VM::readShort() {
  uint8_t high = readByte();
  uint8_t lo = readByte();
  return (uint16_t)((high << 8) | lo);
}

void VM::defineNative(std::string name, NativeFn function) {
  std::shared_ptr<ObjectString> nativeName =
      std::make_shared<ObjectString>(name);
  globals.emplace(nativeName, OBJECT_VAL(std::make_shared<ObjectNative>(
                                  function, nativeName)));
}

Value VM::clockNative(int argCount, size_t index) {
  if (argCount != 0) {
    runtimeError("Expect 0 argument for 'clock', but found %d.", argCount);
  }
  (void)index;
  return NUM_VAL((double)clock() / CLOCKS_PER_SEC);
}

Value VM::substringNative(int argCount, size_t start) {
  if (argCount != 3) {
    runtimeError("Expect 3 arguments for 'substring', but found %d.", argCount);
  }
  Value string = memory.getValueAt(start);
  if (!IS_STRING(string)) {
    runtimeError("Expect a string as first argument for 'substring'.");
  }
  Value startIndex = memory.getValueAt(start + 1);
  Value endIndex = memory.getValueAt(start + 2);
  if (!IS_NUM(startIndex) || !IS_NUM(endIndex)) {
    runtimeError("Indices must be non-negative integers for 'substring'.");
  }
  std::string strVal = AS_STRING(string);
  double startIndexVal = AS_NUM(startIndex);
  double endIndexVal = AS_NUM(endIndex);
  if (floor(startIndexVal) != ceil(startIndexVal) ||
      floor(endIndexVal) != ceil(endIndexVal) || startIndexVal < 0 ||
      endIndexVal < 0) {
    runtimeError("Indices must be non-negative integers for 'substring'.");
  }
  if ((unsigned)startIndexVal >= strVal.size() ||
      startIndexVal >= endIndexVal) {
    return OBJECT_VAL(std::make_shared<ObjectString>(""));
  } else if ((unsigned)endIndexVal >= strVal.size()) {
    return OBJECT_VAL(
        std::make_shared<ObjectString>(strVal.substr((unsigned)startIndexVal)));
  } else {
    double substrSize = endIndexVal - startIndexVal;
    return OBJECT_VAL(std::make_shared<ObjectString>(
        strVal.substr((unsigned)startIndexVal, (unsigned)substrSize)));
  }
}

Value VM::sizeNative(int argCount, size_t start) {
  if (argCount != 1) {
    runtimeError("Expect 1 argument for 'size', but found %d.", argCount);
  }
  Value val = memory.getValueAt(start);

  if (!IS_STRING(val) && !IS_LIST(val))
    runtimeError("Invalid argument for 'size'.");

  if (IS_STRING(val)) {
    unsigned strSize = AS_STRING(val).size();
    return NUM_VAL((double)strSize);
  } else {
    unsigned listSize = AS_OBJECTLIST(val)->size();
    return NUM_VAL((double)listSize);
  }
}

std::shared_ptr<ObjectUpvalue> VM::captureUpvalue(Value* local,
                                                  int localIndex) {
  std::shared_ptr<ObjectUpvalue> prevUpvalue = nullptr;
  std::shared_ptr<ObjectUpvalue> upvalue = openUpvalues;
  while (upvalue != nullptr && upvalue->getLocationIndex() > localIndex) {
    prevUpvalue = upvalue;
    upvalue = upvalue->next;
  }
  if (upvalue != nullptr && upvalue->getLocation() == local) {
    return upvalue;
  }

  std::shared_ptr<ObjectUpvalue> toReturn =
      std::make_shared<ObjectUpvalue>(localIndex, local);
  toReturn->next = upvalue;

  if (prevUpvalue == nullptr) {
    openUpvalues = toReturn;
  } else {
    prevUpvalue->next = toReturn;
  }

  return toReturn;
}

void VM::closeUpvalues(int lastIndex) {
  while (openUpvalues != nullptr &&
         openUpvalues->getLocationIndex() >= lastIndex) {
    std::shared_ptr<ObjectUpvalue> upvalue = openUpvalues;
    upvalue->closed = *(upvalue->getLocation());
    upvalue->location = &(upvalue->closed.value());
    openUpvalues = upvalue->next;
  }
}

void VM::defineField(std::shared_ptr<ObjectString> name) {
  std::shared_ptr<ObjectClass> objClass = AS_CLASS(memory.top());
  objClass->setField(name, (AccessModifier)readByte());
}

void VM::defineMethod(std::shared_ptr<ObjectString> name) {
  Value method = memory.top();
  std::shared_ptr<ObjectClass> classObj =
      AS_CLASS(memory.getValueAt(memory.size() - 2));
  classObj->setMethod(name, method);
  memory.pop();
}

bool VM::bindMethod(const ObjectClass& instanceOf,
                    std::shared_ptr<ObjectString> name) {
  const Value* method = instanceOf.getMethod(name);
  if (method == nullptr) {
    runtimeError("Undefined property '%s'.", name->getString().c_str());
    return false;
  }

  std::shared_ptr<ObjectBoundMethod> bound =
      std::make_shared<ObjectBoundMethod>(memory.top(), AS_CLOSURE(*method));
  memory.pop();
  memory.push(OBJECT_VAL(bound));
  return true;
}

void VM::invoke(std::shared_ptr<ObjectString> name, int argCount) {
  Value receiver = memory.getValueAt(memory.size() - 1 - argCount);

  if (!IS_INSTANCE(receiver)) {
    runtimeError("Only instances have methods.");
  }

  std::shared_ptr<ObjectInstance> instance = AS_INSTANCE(receiver);

  const Value* field = instance->getField(name);

  if (field != nullptr) {
    memory.setValueAt(*field, memory.size() - 1 - argCount);
    callValue(*field, argCount);
  }

  invokeFromClass(instance->getInstanceOf(), name, argCount);
}

void VM::invokeFromClass(const ObjectClass& instanceOf,
                         std::shared_ptr<ObjectString> name, int argCount) {
  const Value* method = instanceOf.getMethod(name);

  if (method == nullptr) {
    runtimeError("Undefined property '%s'.", name->getString().c_str());
  }

  call(AS_CLOSURE(*method), argCount);
}