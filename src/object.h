#pragma once

#include <string>

#include "value.h"

#define OBJECT_TYPE(value) (AS_OBJECT(value)->getType())

#define IS_STRING(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_STRING)

#define AS_OBJECTSTRING(value) ((ObjectString*)AS_OBJECT(value))
#define AS_STRING(value) (((ObjectString*)AS_OBJECT(value))->getString())

enum ObjectType {
  OBJECT_STRING,
};

class Object {
 protected:
  const ObjectType type;

 public:
  Object(ObjectType type);
  ObjectType getType() const;
};

class ObjectString : public Object {
  std::string str;

 public:
  ObjectString(std::string str);
  std::string getString() const;
};
