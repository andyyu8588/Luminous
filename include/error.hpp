/*
 * Copyright (c) Andy Yu and Yunze Zhou
 * Luminous implementation code written by Yun Ze Zhou and Andy Yu.
 * Sharing and altering of the source code is restricted under the MIT License.
 */

#pragma once

#include <string>

extern bool errorOccured;

extern bool panicMode;

void error(int line, const std::string& message, const std::string& file);
