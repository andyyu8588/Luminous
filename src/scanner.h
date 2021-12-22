/*
This is the scanner header file used to tokenize Luminous code. 
Created by Yun Ze Zhou and Andy Yu.
*/

#pragma once
#include <string>
#include <vector>

/*
Exceptions: 

SFAULT = syntax error
MFAULT = math error

*/

struct Token {
    std::string token;
    std::string value;
};

class Scanner {

public:
    Scanner();
    std::vector<Token> tokenize();
};
