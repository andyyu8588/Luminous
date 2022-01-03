#pragma once

#include <memory>
#include <string>

#include "value.h"

#define OBJECT_TYPE(value) (AS_OBJECT(value)->getType())

#define IS_STRING(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_STRING)

#define AS_OBJECTSTRING(value) \
  (std::static_pointer_cast<ObjectString>(AS_OBJECT(value)))
#define AS_STRING(value) \
  ((std::static_pointer_cast<ObjectString>(AS_OBJECT(value)))->getString())

enum ObjectType {
  OBJECT_STRING,
};

class Object {
 protected:
  const ObjectType type;

 public:
  Object(ObjectType type);
  ObjectType getType() const;

  void printObject() const;
};

class ObjectString : public Object {
  std::string str;
  size_t hash;

 public:
  ObjectString(std::string str);
  const std::string& getString() const;
  size_t getHash() const;

  struct Hash {
    size_t operator()(const std::shared_ptr<ObjectString>&) const;
  };

  struct Comparator {
    bool operator()(const std::shared_ptr<ObjectString>& a,
                    const std::shared_ptr<ObjectString>& b) const;
  };
};
