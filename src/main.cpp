#include <fstream>
#include <iostream>

#include "chunk.hpp"
#include "compiler.hpp"
#include "debug.hpp"
#include "vm.hpp"

static void run(Compiler& compiler, VM& vm, const std::string& code,
                std::string currentFile) {
  try {
    compiler.compile(code, currentFile);
    auto function = compiler.getFunction();
    if (function->empty()) return;
    vm.interpret(function);
    compiler.migrate();
  } catch (const CompilerException& e) {
    return;
  } catch (const VMException& e) {
    compiler.tempClear();
    return;
  }
}

static void repl(Compiler& compiler, VM& vm) {
  std::string input;
  std::ifstream versionFile("VERSION");
  std::string version((std::istreambuf_iterator<char>(versionFile)),
                      std::istreambuf_iterator<char>());

  std::cout << "Luminous " + version + " REPL" << std::endl;
  while (true) {
    std::cout << "☀️  ";

    if (!std::getline(std::cin, input)) {
      std::cout << std::endl;
      break;
    }
    if (!input.empty()) {
      run(compiler, vm, input, "<repl>");
    }
  }
}

static void runFile(Compiler& compiler, VM& vm, char* path) {
  std::ifstream sourceFile(path);
  std::string code((std::istreambuf_iterator<char>(sourceFile)),
                   std::istreambuf_iterator<char>());
  if (!code.empty()) {
    run(compiler, vm, code, std::string(path));
  }
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
