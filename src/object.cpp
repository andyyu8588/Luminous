/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yun Ze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

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
    case OBJECT_LIST: {
      ((ObjectList*)this)->printList();
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

bool ObjectFunction::empty() const { return chunk.getBytecodeSize() == 0; }

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

const AccessModifier* ObjectClass::getAccessModifier(
    std::shared_ptr<ObjectString> name) const {
  if (fields.find(name) == fields.end()) {
    if (methods.find(name) == methods.end()) {
      return nullptr;
    }
    return &(methods.find(name)->second.second);
  }
  return &(fields.find(name)->second);
}

const Value* ObjectClass::getMethod(std::shared_ptr<ObjectString> name) const {
  if (methods.find(name) == methods.end()) return nullptr;
  return &(methods.find(name)->second.first);
}

const std::unordered_map<std::shared_ptr<ObjectString>, AccessModifier,
                         ObjectString::Hash, ObjectString::Comparator>&
ObjectClass::getFields() const {
  return fields;
}

void ObjectClass::setField(std::shared_ptr<ObjectString> name,
                           AccessModifier accessModifier) {
  fields.insert_or_assign(name, accessModifier);
}

void ObjectClass::setMethod(std::shared_ptr<ObjectString> name, Value method,
                            AccessModifier am) {
  methods.insert_or_assign(name, std::make_pair(method, am));
}

void ObjectClass::copyMethodsFrom(const ObjectClass& parent) {
  for (auto& it : parent.methods) {
    if (it.second.second != AccessModifier::ACCESS_PRIVATE) {
      methods.insert_or_assign(it.first, it.second);
    }
  }
}

void ObjectClass::copyFieldsFrom(const ObjectClass& parent) {
  for (auto& it : parent.fields) {
    if (it.second != AccessModifier::ACCESS_PRIVATE) {
      fields.insert_or_assign(it.first, it.second);
    }
  }
}

ObjectInstance::ObjectInstance(const ObjectClass& instanceOf)
    : Object(OBJECT_INSTANCE), instanceOf{instanceOf} {
  for (auto& it : instanceOf.getFields()) {
    setField(it.first, NULL_VAL);
  }
}

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

ObjectList::ObjectList() : Object(OBJECT_LIST) {}
ObjectList::ObjectList(std::vector<Value> list)
    : Object{OBJECT_LIST}, list{list} {}
void ObjectList::add(Value v) { list.push_back(v); }
Value ObjectList::get(int i) const { return list[i]; }

void ObjectList::printList() const {
  std::cout << "[";
  for (unsigned i = 0; i < list.size(); i++) {
    list.at(i).printValue();
    if (i + 1 != list.size()) {
      std::cout << ", ";
    }
  }
  std::cout << "]";
}

size_t ObjectList::size() const { return list.size(); }

void ObjectList::set(Value v, int i) { list[i] = v; }
