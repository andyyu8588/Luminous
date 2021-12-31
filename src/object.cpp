#include "object.h"

#include <iostream>

Object::Object(ObjectType type) : type{type} {}

ObjectType Object::getType() const { return type; }

ObjectString::ObjectString(std::string str) : Object(OBJECT_STRING), str{str} {}

const std::string& ObjectString::getString() const { return str; }

void Object::printObject() const {
  switch (type) {
    case OBJECT_STRING: {
      std::cout << ((ObjectString*)this)->getString();
      break;
    }
  }
}
