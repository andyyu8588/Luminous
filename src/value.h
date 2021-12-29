#pragma once

#include <variant>

// actual -> Value
#define BOOL_VAL(value) ((Value){VAL_BOOL, value})
#define NULL_VAL ((Value){VAL_NULL, (double)0})
#define NUM_VAL(value) ((Value){VAL_NUM, value})

// Value -> actual (no need for null)
#define AS_BOOL(value) (std::get<bool>((value).as))
#define AS_NUM(value) (std::get<double>((value).as))

// Type checks
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NULL(value) ((value).type == VAL_NULL)
#define IS_NUM(value) ((value).type == VAL_NUM)

enum ValueType { VAL_BOOL, VAL_NULL, VAL_NUM };

struct Value {
  ValueType type;
  std::variant<bool, double> as;
};

namespace ValueTools {
bool valuesEqual(Value a, Value b);
void printValue(Value value);
}  // namespace ValueTools
