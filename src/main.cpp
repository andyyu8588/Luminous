#include <fstream>
#include <iostream>

#include "chunk.h"
#include "compiler.h"
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

    // vm.interpret(input);
  }
}

static void runFile(char* path) {
  std::ifstream sourceFile(path);
  std::string code((std::istreambuf_iterator<char>(sourceFile)),
                   std::istreambuf_iterator<char>());

  Compiler compiler = Compiler(code);
  compiler.compile();
  VM vm;
  InterpretResult interpretResult = vm.interpret(compiler.getCurrentChunk());
  std::cout << interpretResult << std::endl;
}

int main(int argc, char* argv[]) {
  int counter = 0;
  char* path;
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (counter == 1) {
        path = argv[i];
      }
      counter++;
    }
  }
  if (counter == 1) {
    repl();
  } else if (counter == 2) {
    runFile(path);
  } else {
    std::cerr << "Usage ./luminous [path]" << std::endl;
    return 1;
  }
  return 0;
}
