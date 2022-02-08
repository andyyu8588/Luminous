#include "object.hpp"

#include <iostream>

Object::Object(ObjectType type) : type{type} {}

ObjectType Object::getType() const { return type; }

ObjectString::ObjectString(const std::string& str)
    : Object(OBJECT_STRING), str{str}, hash{std::hash<std::string>{}(str)} {}

const std::string& ObjectString::getString() const { return str; }

size_t ObjectString::getHash() const { return hash; }

void Object::printObject() const {
  switch (type) {
    case OBJECT_BOUND_METHOD: {
      ((ObjectBoundMethod*)this)->getMethod()->getFunction()->printObject();
      break;
    }
    case OBJECT_CLASS: {
      std::cout << ((ObjectClass*)this)->getName().getString();
      break;
    }
    case OBJECT_CLOSURE: {
      ((ObjectClosure*)this)->getFunction()->printObject();
      break;
    }
    case OBJECT_FUNCTION: {
      if (((ObjectFunction*)this)->getName() == nullptr) {
        std::cout << "<script>";
      } else {
        std::cout << ((ObjectFunction*)this)->getName()->getString();
      }
      break;
    }
    case OBJECT_INSTANCE: {
      ((ObjectInstance*)this)->getInstanceOf().printObject();
      std::cout << " instance";
#ifdef DEBUG
      std::cout << std::endl;
      std::cout << "Fields:" << std::endl;
      for (auto& it : ((ObjectInstance*)this)->getFields()) {
        std::cout << "Name: " << it.first->getString() << std::endl;
        std::cout << "Value: ";
        it.second.printValue();
        std::cout << std::endl;
      }
#endif
      break;
    }
    case OBJECT_STRING: {
      std::cout << ((ObjectString*)this)->getString();
      break;
    }
    case OBJECT_NATIVE: {
      std::cout << ((ObjectNative*)this)->getName()->getString();
      break;
    }
    case OBJECT_UPVALUE: {
      std::cout << "upvalue" << std::endl;
      break;
    }
    case OBJECT_ARRAY: {
      ((ObjectArray*)this)->printArray();
    }
  }
}

size_t ObjectString::Hash::operator()(
    const std::shared_ptr<ObjectString>& a) const {
  return a->getHash();
}

bool ObjectString::Comparator::operator()(
    const std::shared_ptr<ObjectString>& a,
    const std::shared_ptr<ObjectString>& b) const {
  return a->getString() == b->getString();
}

ObjectFunction::ObjectFunction(std::shared_ptr<ObjectString> name)
    : Object(OBJECT_FUNCTION), name{name} {}

const std::shared_ptr<ObjectString> ObjectFunction::getName() const {
  return name;
}

Chunk& ObjectFunction::getChunk() { return chunk; }

void ObjectFunction::increaseArity() { arity++; }

int ObjectFunction::getArity() const { return arity; }

ObjectNative::ObjectNative(const NativeFn function,
                           const std::shared_ptr<ObjectString> name)
    : Object(OBJECT_NATIVE), function{function}, name{name} {}

NativeFn ObjectNative::getFunction() { return function; }

std::shared_ptr<ObjectString> ObjectNative::getName() { return name; }

ObjectClosure::ObjectClosure(std::shared_ptr<ObjectFunction> function)
    : Object(OBJECT_CLOSURE), function{function} {
  upvalueCount = function->getUpvalueCount();
}

std::shared_ptr<ObjectFunction> ObjectClosure::getFunction() {
  return function;
}

size_t ObjectClosure::getUpvaluesSize() const { return upvalues.size(); }

void ObjectClosure::setUpvalue(int index,
                               std::shared_ptr<ObjectUpvalue> upvalue) {
  upvalues[index] = upvalue;
}

std::shared_ptr<ObjectUpvalue> ObjectClosure::getUpvalue(int index) const {
  return upvalues[index];
}

ObjectUpvalue::ObjectUpvalue(int locationIndex, Value* location)
    : Object(OBJECT_UPVALUE),
      locationIndex{locationIndex},
      location{location} {}

Value* ObjectUpvalue::getLocation() const { return location; }

void ObjectFunction::increateUpvalueCount() { upvalueCount++; }

int ObjectFunction::getUpvalueCount() const { return upvalueCount; }

int ObjectClosure::getUpvalueCount() const { return upvalueCount; }

void ObjectClosure::addUpvalue(std::shared_ptr<ObjectUpvalue> upvalue) {
  upvalues.push_back(upvalue);
}

int ObjectUpvalue::getLocationIndex() const { return locationIndex; }

ObjectClass::ObjectClass(const std::string& name)
    : Object(OBJECT_CLASS), name{ObjectString(name)} {}

const ObjectString& ObjectClass::getName() const { return name; }

const Value* ObjectClass::getMethod(std::shared_ptr<ObjectString> name) const {
  if (methods.find(name) == methods.end()) return nullptr;
  return &(methods.find(name)->second);
}

void ObjectClass::setMethod(std::shared_ptr<ObjectString> name, Value method) {
  methods.insert_or_assign(name, method);
}

void ObjectClass::copyMethodsFrom(const ObjectClass& parent) {
  for (auto& it : parent.methods) {
    methods.insert_or_assign(it.first, it.second);
  }
}

ObjectInstance::ObjectInstance(const ObjectClass& instanceOf)
    : Object(OBJECT_INSTANCE), instanceOf{instanceOf} {}

const ObjectClass& ObjectInstance::getInstanceOf() const { return instanceOf; }

const Value* ObjectInstance::getField(
    std::shared_ptr<ObjectString> name) const {
  if (fields.find(name) == fields.end()) return nullptr;
  return &(fields.find(name)->second);
}

void nullDeleter(ObjectInstance* toVoid) { (void)toVoid; }

void ObjectInstance::setField(std::shared_ptr<ObjectString> name, Value value) {
  if (IS_INSTANCE(value) && AS_INSTANCE(value).get() == this) {
    std::shared_ptr<ObjectInstance> copy(AS_INSTANCE(value).get(),
                                         &nullDeleter);
    fields.insert_or_assign(name, OBJECT_VAL(copy));
  } else {
    fields.insert_or_assign(name, value);
  }
}

ObjectBoundMethod::ObjectBoundMethod(Value receiver,
                                     std::shared_ptr<ObjectClosure> method)
    : Object(OBJECT_BOUND_METHOD), receiver{receiver}, method{method} {}

Value ObjectBoundMethod::getReceiver() const { return receiver; }

std::shared_ptr<ObjectClosure> ObjectBoundMethod::getMethod() const {
  return method;
}

ObjectArray::ObjectArray() : Object(OBJECT_ARRAY) {}
void ObjectArray::add(Value v) { array.push_back(v); }
Value ObjectArray::get(int i) { return array[i]; }

void ObjectArray::printArray() {
  std::cout << "[";
  for (unsigned i = 0; i < array.size(); i++) {
    array.at(i).printValue();
    if (i + 1 != array.size()) {
      std::cout << ", ";
    }
  }
  std::cout << "]";
}

size_t ObjectArray::size() const { return array.size(); }

void ObjectArray::set(Value v, int i) { array[i] = v; }
