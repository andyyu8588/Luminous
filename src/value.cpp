#include "value.h"

#include <iostream>

bool ValueTools::valuesEqual(Value a, Value b) {
  if (a.type != b.type) return false;

  switch (a.type) {
    case VAL_BOOL:
      return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NULL:
      return true;
    case VAL_NUM:
      return AS_NUM(a) == AS_NUM(b);
    default:
      return false;  // unreachable
  }
}

void ValueTools::printValue(Value value) {
  switch (value.type) {
    case VAL_NUM:
      std::cout << AS_NUM(value);
      break;
    case VAL_BOOL:
      std::cout << AS_BOOL(value);
      break;
    case VAL_NULL:
      std::cout << "null";
      break;
  }
}
