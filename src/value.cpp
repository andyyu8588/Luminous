#include "value.h"

#include <iostream>

#include "object.h"

bool Value::operator==(const Value& compared) const {
  if (type != compared.type) return false;

  switch (type) {
    case VAL_BOOL: {
      return AS_BOOL(*this) == AS_BOOL(compared);
    }
    case VAL_NULL: {
      return true;
    }
    case VAL_NUM: {
      return AS_NUM(*this) == AS_NUM(compared);
    }
    case VAL_OBJECT: {
      const std::string& a = AS_STRING(*this);
      const std::string& b = AS_STRING(compared);
      return a == b;
    }
    default: {
      return false;  // unreachable
    }
  }
}

void Value::printValue() const {
  switch (type) {
    case VAL_NUM:
      std::cout << AS_NUM(*this);
      break;
    case VAL_BOOL:
      std::cout << AS_BOOL(*this);
      break;
    case VAL_NULL:
      std::cout << "null";
      break;
    case VAL_OBJECT:
      AS_OBJECT(*this)->printObject();
      break;
  }
}

ValueType Value::getType() const { return type; }

std::variant<bool, double, std::shared_ptr<Object>> Value::getAs() const {
  return as;
}
