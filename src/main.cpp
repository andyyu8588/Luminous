#include <fstream>
#include <iostream>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

static void run(Compiler& compiler, VM& vm, const std::string& code) {
  try {
    compiler.compile(code);
  } catch (const std::exception& e) {
    return;
  }
  InterpretResult interpretResult = vm.interpret(compiler.getCurrentChunk());
  std::cout << interpretResult << std::endl;
}

static void repl(Compiler& compiler, VM& vm) {
  std::string input;

  while (true) {
    std::cout << "> ";

    if (!std::getline(std::cin, input)) {
      std::cout << std::endl;
      break;
    }

    run(compiler, vm, input);
  }
}

static void runFile(Compiler& compiler, VM& vm, char* path) {
  std::ifstream sourceFile(path);
  std::string code((std::istreambuf_iterator<char>(sourceFile)),
                   std::istreambuf_iterator<char>());
  run(compiler, vm, code);
}

int main(int argc, char* argv[]) {
  int argcWithoutFlags = 0;
  char* path;
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] != '-') {
      if (argcWithoutFlags == 1) {
        path = argv[i];
      }
      argcWithoutFlags++;
    }
  }

  Compiler compiler;
  VM vm;

  // interpret depending on num args
  if (argcWithoutFlags == 1) {
    repl(compiler, vm);
  } else if (argcWithoutFlags == 2) {
    runFile(compiler, vm, path);
  } else {
    std::cerr << "Usage ./luminous [path]" << std::endl;
    return 1;
  }
  return 0;
}
