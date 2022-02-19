#include "error.hpp"

#include <iostream>

bool errorOccured = false;

bool panicMode = false;

void error(int line, std::string message, std::string file) {
  if (!panicMode) {
    std::cerr << message << " (line " << line << " in file '" + file + "')"
              << std::endl;
    std::cerr << "(Compile Error)" << std::endl;
    errorOccured = true;
    panicMode = true;
  }
}
