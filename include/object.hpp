#pragma once

#include <memory>
#include <optional>
#include <string>

#include "chunk.hpp"

#define OBJECT_TYPE(value) (AS_OBJECT(value)->getType())

#define IS_CLOSURE(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_CLOSURE)
#define IS_FUNCTION(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_FUNCTION)
#define IS_NATIVE(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_NATIVE)
#define IS_STRING(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_STRING)

#define AS_CLOSURE(value) \
  (std::static_pointer_cast<ObjectClosure>(AS_OBJECT(value)))
#define AS_FUNCTION(value) \
  (std::static_pointer_cast<ObjectFunction>(AS_OBJECT(value)))
#define AS_NATIVE(value) \
  (std::static_pointer_cast<ObjectNative>(AS_OBJECT(value)))
#define AS_OBJECTSTRING(value) \
  (std::static_pointer_cast<ObjectString>(AS_OBJECT(value)))
#define AS_STRING(value) \
  ((std::static_pointer_cast<ObjectString>(AS_OBJECT(value)))->getString())

enum ObjectType {
  OBJECT_CLOSURE,
  OBJECT_FUNCTION,
  OBJECT_NATIVE,
  OBJECT_STRING,
  OBJECT_UPVALUE
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
  ObjectString(const std::string& str);
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

class ObjectFunction : public Object {
  int arity;
  Chunk chunk;
  std::shared_ptr<ObjectString> name;
  int upvalueCount = 0;

 public:
  ObjectFunction(std::shared_ptr<ObjectString> name);
  const std::shared_ptr<ObjectString> getName() const;
  Chunk& getChunk();
  void increaseArity();
  int getArity() const;
  void increateUpvalueCount();
  int getUpvalueCount() const;
};

typedef Value (*NativeFn)(int argCount, size_t start);
class ObjectNative : public Object {
  const NativeFn function;
  const std::shared_ptr<ObjectString> name;

 public:
  ObjectNative(const NativeFn function,
               const std::shared_ptr<ObjectString> name);
  NativeFn getFunction();
  std::shared_ptr<ObjectString> getName();
};

class ObjectUpvalue : public Object {
  int locationIndex;

 public:
  std::shared_ptr<ObjectUpvalue> next = nullptr;
  std::optional<Value> closed;
  Value* location;
  ObjectUpvalue(Value* location, int locationIndex);
  Value* getLocation() const;
  int getLocationIndex() const;
};

class ObjectClosure : public Object {
  std::shared_ptr<ObjectFunction> function;
  std::vector<std::shared_ptr<ObjectUpvalue>> upvalues;
  int upvalueCount;

 public:
  ObjectClosure(std::shared_ptr<ObjectFunction>);
  std::shared_ptr<ObjectFunction> getFunction();
  size_t getUpvaluesSize() const;
  void setUpvalue(int, std::shared_ptr<ObjectUpvalue>);
  void addUpvalue(std::shared_ptr<ObjectUpvalue>);
  std::shared_ptr<ObjectUpvalue> getUpvalue(int) const;
  int getUpvalueCount() const;
};