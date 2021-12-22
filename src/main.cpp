#include <iostream>
#include <fstream>
#include "scanner.h"


int main(int argc, char *argv[]) {
	(void) argc;
	std::ifstream sourceFile(argv[1]);
	std::string code((std::istreambuf_iterator<char>(sourceFile)),
                 std::istreambuf_iterator<char>());

	Scanner *scanner = new Scanner(code);

	std::vector<std::shared_ptr<Token>> tokens = scanner->tokenize();

	for(auto token : tokens) {
		Token::printToken(token);
	}

	return 0;
}
