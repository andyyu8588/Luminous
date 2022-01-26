#include "object.hpp"

#include <iostream>

Object::Object(ObjectType type) : type{type} {}

ObjectType Object::getType() const { return type; }

ObjectString::ObjectString(const std::string& str)
    : Object(OBJECT_STRING), str{str} {
  hash = std::hash<std::string>{}(str);
}

const std::string& ObjectString::getString() const { return str; }

size_t ObjectString::getHash() const { return hash; }

void Object::printObject() const {
  switch (type) {
    case OBJECT_FUNCTION: {
      if (((ObjectFunction*)this)->getName() == nullptr) {
        std::cout << "<script>";
      } else {
        std::cout << ((ObjectFunction*)this)->getName()->getString();
      }
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
    : Object(OBJECT_FUNCTION), arity{0}, name{name} {}

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