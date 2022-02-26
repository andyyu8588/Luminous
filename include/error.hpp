#pragma once

#include <string>

extern bool errorOccured;

extern bool panicMode;

void error(int line, const std::string& message, const std::string& file);
