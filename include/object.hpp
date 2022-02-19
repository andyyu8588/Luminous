#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "chunk.hpp"
#include "value.hpp"

#define OBJECT_TYPE(value) (AS_OBJECT(value)->getType())

#define IS_BOUND_METHOD(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_BOUND_METHOD)
#define IS_CLASS(value) (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_CLASS)
#define IS_CLOSURE(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_CLOSURE)
#define IS_FUNCTION(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_FUNCTION)
#define IS_INSTANCE(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_INSTANCE)
#define IS_NATIVE(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_NATIVE)
#define IS_STRING(value) \
  (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_STRING)
#define IS_LIST(value) (IS_OBJECT(value) && OBJECT_TYPE(value) == OBJECT_LIST)

#define AS_BOUND_METHOD(value) \
  (std::static_pointer_cast<ObjectBoundMethod>(AS_OBJECT(value)))
#define AS_CLASS(value) \
  (std::static_pointer_cast<ObjectClass>(AS_OBJECT(value)))
#define AS_CLOSURE(value) \
  (std::static_pointer_cast<ObjectClosure>(AS_OBJECT(value)))
#define AS_FUNCTION(value) \
  (std::static_pointer_cast<ObjectFunction>(AS_OBJECT(value)))
#define AS_INSTANCE(value) \
  (std::static_pointer_cast<ObjectInstance>(AS_OBJECT(value)))
#define AS_NATIVE(value) \
  (std::static_pointer_cast<ObjectNative>(AS_OBJECT(value)))
#define AS_OBJECTSTRING(value) \
  (std::static_pointer_cast<ObjectString>(AS_OBJECT(value)))
#define AS_STRING(value) \
  ((std::static_pointer_cast<ObjectString>(AS_OBJECT(value)))->getString())
#define AS_OBJECTLIST(value) \
  (std::static_pointer_cast<ObjectList>(AS_OBJECT(value)))

enum ObjectType {
  OBJECT_BOUND_METHOD,
  OBJECT_CLASS,
  OBJECT_CLOSURE,
  OBJECT_FUNCTION,
  OBJECT_INSTANCE,
  OBJECT_NATIVE,
  OBJECT_STRING,
  OBJECT_UPVALUE,
  OBJECT_LIST
};

class Object {
 protected:
  const ObjectType type;

 public:
  Object(ObjectType type);

  // getters
  ObjectType getType() const;

  void printObject() const;
};

class ObjectString : public Object {
  const std::string str;
  const size_t hash;

 public:
  ObjectString(const std::string& str);

  // getters
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
  int arity = 0;
  Chunk chunk;
  std::shared_ptr<ObjectString> name;
  int upvalueCount = 0;

 public:
  ObjectFunction(std::shared_ptr<ObjectString> name);

  // getters
  const std::shared_ptr<ObjectString> getName() const;
  Chunk& getChunk();
  int getArity() const;
  int getUpvalueCount() const;

  void increaseArity();
  void increateUpvalueCount();
  bool empty() const;
};

using NativeFn = std::function<Value(int, size_t)>;
class ObjectNative : public Object {
  const NativeFn function;
  const std::shared_ptr<ObjectString> name;

 public:
  ObjectNative(const NativeFn function,
               const std::shared_ptr<ObjectString> name);

  // getters
  NativeFn getFunction();
  std::shared_ptr<ObjectString> getName();
};

class ObjectUpvalue : public Object {
  const int locationIndex;

 public:
  std::shared_ptr<ObjectUpvalue> next = nullptr;
  std::optional<Value> closed;
  Value* location;

  ObjectUpvalue(int locationIndex, Value* location);

  // getters
  Value* getLocation() const;
  int getLocationIndex() const;
};

class ObjectClosure : public Object {
  std::shared_ptr<ObjectFunction> function;
  std::vector<std::shared_ptr<ObjectUpvalue>> upvalues;
  int upvalueCount;

 public:
  ObjectClosure(std::shared_ptr<ObjectFunction>);

  // getters
  size_t getUpvaluesSize() const;
  std::shared_ptr<ObjectUpvalue> getUpvalue(int) const;
  int getUpvalueCount() const;

  void setUpvalue(int, std::shared_ptr<ObjectUpvalue>);
  void addUpvalue(std::shared_ptr<ObjectUpvalue>);
  std::shared_ptr<ObjectFunction> getFunction();
};

class ObjectClass : public Object {
  ObjectString name;
  std::unordered_map<std::shared_ptr<ObjectString>, Value, ObjectString::Hash,
                     ObjectString::Comparator>
      methods;

 public:
  ObjectClass(const std::string& name);

  // getters
  const ObjectString& getName() const;
  const Value* getMethod(std::shared_ptr<ObjectString>) const;

  void setMethod(std::shared_ptr<ObjectString>, Value);

  // for inheritance
  void copyMethodsFrom(const ObjectClass& parent);
};

class ObjectInstance : public Object {
  const ObjectClass& instanceOf;
  std::unordered_map<std::shared_ptr<ObjectString>, Value, ObjectString::Hash,
                     ObjectString::Comparator>
      fields;

 public:
  ObjectInstance(const ObjectClass& instanceOf);

  const ObjectClass& getInstanceOf() const;
  const Value* getField(std::shared_ptr<ObjectString> name) const;
  void setField(std::shared_ptr<ObjectString> name, Value value);

#ifdef DEBUG
  std::unordered_map<std::shared_ptr<ObjectString>, Value, ObjectString::Hash,
                     ObjectString::Comparator>&
  getFields() {
    return fields;
  }
#endif
};

class ObjectBoundMethod : public Object {
  const Value receiver;
  std::shared_ptr<ObjectClosure> method;

 public:
  ObjectBoundMethod(Value receiver, std::shared_ptr<ObjectClosure> method);

  Value getReceiver() const;
  std::shared_ptr<ObjectClosure> getMethod() const;
};

class ObjectList : public Object {
  std::vector<Value> list;

 public:
  ObjectList();
  ObjectList(std::vector<Value>);

  void add(Value v);
  void set(Value v, int i);
  Value get(int i) const;

  void printList() const;
  size_t size() const;
};