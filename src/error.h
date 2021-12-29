#pragma once

#include <string>

extern bool errorOccured;

extern bool panicMode;

void error(int line, std::string message);
