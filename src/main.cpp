#include <iostream>

#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl(VM& vm) {
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

static void runFile(VM& vm, char* path) {
  // TODO
  (void)vm;
  (void)path;
}

int main(int argc, char* argv[]) {
  VM vm;

  if (argc == 1) {
    repl(vm);
  } else if (argc == 2) {
    runFile(vm, argv[1]);
  } else {
    std::cout << "Usage: luminous [path]" << std::endl;
    return 1;
  }

  return 0;
}
