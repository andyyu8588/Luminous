#include "error.h"

#include <iostream>

bool errorOccured = false;

bool panicMode = false;

void error(int line, std::string message) {
  if (!panicMode) {
    std::cerr << "ERROR: " << message << " (line " << line << ")." << std::endl;
    errorOccured = true;
    panicMode = true;
  }
}

void disablePanicMode() { panicMode = false; }
