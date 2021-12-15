#pragma once
#include <iostream>
#include <string>

static bool errorOccured = false;

class Error {

    static void error(int line, std::string message) {
        std::cerr << "ERROR: " << message << " (line " << line << ")." << std::endl;
        errorOccured = true;
    }
}
