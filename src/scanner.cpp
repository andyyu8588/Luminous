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

    // preprocessing token map
    // Parentheses:
    tokenMap.emplace("(", "LPAREN");
    tokenMap.emplace(")", "RPAREN");
    tokenMap.emplace("{", "LBRACE");
    tokenMap.emplace("}", "RBRACE");
    tokenMap.emplace("[", "LBRACK");
    tokenMap.emplace("]", "RBRACK");
    tokenMap.emplace("\"", "QUOTE");
    
    // Operators:
    tokenMap.emplace("=", "BECOMES");
    tokenMap.emplace(".", "DOT");
    tokenMap.emplace(",", "COMMA");
    tokenMap.emplace("+", "PLUS");
    tokenMap.emplace("-", "MINUS");
    tokenMap.emplace("*", "STAR");
    tokenMap.emplace("/", "SLASH");
    tokenMap.emplace(";", "SEMI");
    
    // Comparators:
    tokenMap.emplace("equals", "EQ");
    tokenMap.emplace("and", "AND");
    tokenMap.emplace("or", "OR");
    tokenMap.emplace("not", "NOT");
    tokenMap.emplace("<", "LT");
    tokenMap.emplace(">", "GT");
    tokenMap.emplace("<=", "LE");
    tokenMap.emplace(">=", "GE");
    
    // Control flow:
    tokenMap.emplace("if", "IF");
    tokenMap.emplace("else", "ELSE");
    tokenMap.emplace("while", "WHILE");
    tokenMap.emplace("return", "RETURN");
    
    // Built-in functions:
    tokenMap.emplace("print", "PRINT");
    tokenMap.emplace("addr", "ADDR");
    tokenMap.emplace("at", "AT");

    while(std::cin >> curStr) {
        std::string word = "";
        // separating id and symbol part:
        for(unsigned int i = 0; i < curStr.size(); i++) {
            // if current character is a valid symbol token:
            std::string curCharStr = std::string(1, curStr.at(i));
            if(tokenMap.find(curCharStr) != tokenMap.end()) {
                // need to first tokenize the id part if there is a leading id part:
                // if leading character is an integer (then maybe it's an integer):
                if(word.size() >= 1) {
                    if(word.at(0) >= 48 && word.at(0) <= 57) {
                        try {
                            std::size_t index = 0;
                            int parsed = std::stoi(word, &index);   // for out of bounds detection
                            (void) parsed; // to get rid of the warnings
                            // if stoi length is not word length, then it means that there are other characters after the integers
                            if(index != word.size()) {
                                std::cerr << "SFAULT: Cannot start an ID name with an integer" << std::endl;
                                throw new std::runtime_error("SFAULT: Cannot start an ID name with an integer");
                            }
                            Token num{"NUM", word};
                            tokens.push_back(num);
                        }
                        catch(std::out_of_range OR) {
                            std::cerr << "MFAULT: Integer out of bounds" << std::endl;
                            throw new std::runtime_error("MFAULT: Integer out of bounds");
                        }
                    }
                    // or maybe it's a build it function
                    else if(tokenMap.find(word) != tokenMap.end()) {
                        Token func{tokenMap[word], word};
                        tokens.push_back(func);
                    }
                    // if not, then it must be an ID
                    else {
                        Token id{"ID", word};
                        tokens.push_back(id);
                    }
                }
                // and then tokenize the symbol part:
                // doubly symbols cases:
                if(curStr.at(i) == '<' && i != curStr.size() - 1 && curStr.at(i + 1) == '=') {
                    Token symbol{"LE", "<="};
                    tokens.push_back(symbol);
                    i++;
                }
                else if(curStr.at(i) == '>' && i != curStr.size() - 1 && curStr.at(i + 1) == '=') {
                    Token symbol{"GE", ">="};
                    tokens.push_back(symbol);
                    i++;
                }
                // TODO: think about how to deal with quotes and comments (either in the tokenizer or in the parser)
                // singly symbols case:
                else {
                    Token symbol{tokenMap[curCharStr], curCharStr};
                    tokens.push_back(symbol);
                }
                word = "";
            }
            // if no separation needed, keep getting the id:
            else {
                word += curStr.at(i);
            }
        }
        
        // processing the last word:
        if(word.size() >= 1) {
            // maybe it's a number
            if(word.at(0) >= 48 && word.at(0) <= 57) {
                try {
                    std::size_t index = 0;
                    int parsed = std::stoi(word, &index);   // for out of bounds detection
                    (void) parsed; // to get rid of the warnings
                    // if stoi length is not word length, then it means that there are other characters after the integers
                    if(index != word.size()) {
                        std::cerr << "SFAULT: Cannot start an ID name with an integer" << std::endl;
                        throw new std::runtime_error("SFAULT: Cannot start an ID name with an integer");
                    }
                    Token num{"NUM", word};
                    tokens.push_back(num);
                }
                catch(std::out_of_range OR) {
                    std::cerr << "MFAULT: Integer out of bounds" << std::endl;
                    throw new std::runtime_error("MFAULT: Integer out of bounds");
                }
            }
            // or an existing token in the map
            else if(tokenMap.find(word) != tokenMap.end()) {
                Token tok{tokenMap[word], word};
                tokens.push_back(tok);
            }
            // or it is simply an ID
            else {
                Token id{"ID", word};
                tokens.push_back(id);
            }
        }
    }

    return tokens;
}
