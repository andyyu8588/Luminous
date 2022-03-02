/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yunze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#pragma once

#include <memory>
#include <variant>

// actual -> Value
#define BOOL_VAL(value) (Value(VAL_BOOL, value))
#define NULL_VAL (Value(VAL_NULL, (double)0))
#define NUM_VAL(value) (Value(VAL_NUM, value))
#define OBJECT_VAL(value) (Value(VAL_OBJECT, value))

// Value -> actual (no need for null)
#define AS_BOOL(value) (std::get<bool>((value).getAs()))
#define AS_NUM(value) (std::get<double>((value).getAs()))
#define AS_OBJECT(value) (std::get<std::shared_ptr<Object>>((value).getAs()))

// Type checks
#define IS_BOOL(value) ((value).getType() == VAL_BOOL)
#define IS_NULL(value) ((value).getType() == VAL_NULL)
#define IS_NUM(value) ((value).getType() == VAL_NUM)
#define IS_OBJECT(value) ((value).getType() == VAL_OBJECT)

class Object;

enum ValueType { VAL_BOOL, VAL_NULL, VAL_NUM, VAL_OBJECT };

class Value {
 private:
  ValueType type;
  std::variant<bool, double, std::shared_ptr<Object>> as;

 public:
  ValueType getType() const;
  std::variant<bool, double, std::shared_ptr<Object>> getAs() const;
  bool operator==(const Value& compared) const;
  Value& operator=(Value other);
  void printValue() const;

  Value(ValueType type, std::variant<bool, double, std::shared_ptr<Object>> as);
  Value(const Value& value);
};
