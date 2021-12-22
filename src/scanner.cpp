#include <iostream>
#include <map>
#include <exception>
#include "scanner.h"

// Scanner implementation for Luminous by Yun Ze Zhou and Andy Yu
// Written by Yun Ze Zhou (responsible for main debugging).

Scanner::Scanner() {}

std::vector<Token> Scanner::tokenize() {
    std::string curStr;
    std::vector<Token> tokens;
    std::map<std::string, std::string> tokenMap;

    // preprocessing token map:
    tokenMap.emplace("{", "LBRACE");
    tokenMap.emplace("}", "RBRACE");
    tokenMap.emplace("[", "LBRACK");
    tokenMap.emplace("]", "RBRACK");
    tokenMap.emplace("(", "LPAREN");
    tokenMap.emplace(")", "RPAREN");
    tokenMap.emplace(",", "COMMA");
    tokenMap.emplace(".", "DOT");
    tokenMap.emplace(";", "SEMI");
    tokenMap.emplace("addr", "ADDR");
    tokenMap.emplace("at", "AT");

    while(std::cin >> curStr) {
        std::string word = "";
        // separating id and brackets part:
        for(int i = 0; i < curStr.size(); i++) {
            // if current character is a valid symbol token:
            std::string curCharStr = std::string(1, curStr.at(i));
            if(tokenMap.find(curCharStr) != tokenMap.end()) {
                // need to first tokenize the id part if there is a leading id part:
                // if leading character is an integer:
                if(word.size() >= 1) {
                    if(word.at(0) >= 48 && word.at(0) <= 57) {
                        try {
                            std::stoi(word);
                            Token num{"NUM", word};
                            tokens.push_back(num);
                        } catch(std::invalid_argument IA) {
                            throw new std::runtime_error("SFAULT: Cannot start an ID with an integer.");
                        } catch(std::out_of_range OR) {
                            throw new std::runtime_error("MFAULT: Integer out of bounds");
                        }
                    }
                    else if(tokenMap.find(word) != tokenMap.end()) {
                        Token func{tokenMap[word], word};
                        tokens.push_back(func);
                    }
                    else {
                        Token id{"ID", word};
                        tokens.push_back(id);
                    }
                }
                // and then tokenize the symbol part:
                Token symbol{tokenMap[curCharStr], curCharStr};
                tokens.push_back(symbol);
                word = "";
            }
            // keep getting the id:
            else {
                word += curStr.at(i);
            }
        }
    }

    return tokens;
}
