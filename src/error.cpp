#include "error.h"

#include <iostream>

bool errorOccured = false;

void error(int line, std::string message) {
  std::cerr << "ERROR: " << message << " (line " << line << ")." << std::endl;
  errorOccured = true;
}
