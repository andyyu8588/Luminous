#include <iostream>
#include "scanner.h"

int main(int argc, char *argv[]) {
	Scanner *scanner = new Scanner();

	std::vector<Token> tokens = scanner->tokenize();
	for(Token token : tokens) {
		std::cout << token.token << std::endl;
	}
	return 0;
}
