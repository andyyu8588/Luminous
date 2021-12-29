#include "object.h"

Object::Object(ObjectType type) : type{type} {}

ObjectType Object::getType() const { return type; }

ObjectString::ObjectString(std::string str) : Object(OBJECT_STRING), str{str} {}

std::string ObjectString::getString() const { return str; }
