#include "error.hpp"

#include <iostream>

bool errorOccured = false;

bool panicMode = false;

void error(int line, std::string message) {
  if (!panicMode) {
    std::cerr << message << " (line " << line << ")" << std::endl;
    std::cerr << "(Compile Error)" << std::endl;
    errorOccured = true;
    panicMode = true;
  }
}
