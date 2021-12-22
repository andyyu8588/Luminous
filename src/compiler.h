#pragma once
#include "scanner.h"

struct Parser {
    Token current;
    Token prev;
}

class Compiler {
    Parser parser;
    Scanner scanner;

    static void init(std::string code);

    void advance();
}
