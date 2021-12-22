#include <iostream>

#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl() {
  VM vm;
  std::string input;

  while (true) {
    std::cout << "> ";

    if (!std::getline(std::cin, input)) {
      std::cout << std::endl;
      break;
    }

    vm.interpret(input);
  }
}

static void runFile(char* path) {
  // TODO
  VM vm;
  (void)path;
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    std::cout << "Usage: luminous [path]" << std::endl;
    return 1;
  }

  return 0;
}
