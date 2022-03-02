/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yunze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#include "error.hpp"

#include <iostream>

bool errorOccured = false;

bool panicMode = false;

void error(int line, const std::string& message, const std::string& file) {
  if (!panicMode) {
    std::cerr << message << " (line " << line << " in file '" + file + "')"
              << std::endl;
    std::cerr << "(Compile Error)" << std::endl;
    errorOccured = true;
    panicMode = true;
  }
}
