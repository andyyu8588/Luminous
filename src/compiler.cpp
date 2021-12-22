#include "compiler.h"

static void init(std::string code) {
    parser = Parser();
    scanner = Scanner(code);
}

void advance() {
    parser.prev = parser.current;

    while(true) {
        parser.current = scanner.
    }
}
