#pragma once
#include <string>

enum TokenType {

    // Single symbol tokens:
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACK, RBRACK,
    BECOMES, DOT, MINUS, PLUS,
    COMMA, STAR, SLASH, SEMI,

    // Single and double symbol tokens:
    LT, GT, LE, GE,

    // Literals:
    ID, NUM, STRING,

    // Keywords:
    EQ, AND, OR, NOT, IF, 
    ELSE, WHILE, RETURN, PRINT, 
    ADDR, AT,

};


class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, std::string lexeme, int line) {
        this->type = type;
        this->lexeme = lexeme;
        this->line = line;
    }

    // FOR TESTING ONLY:
    static void printToken(auto token) {
        TokenType type = token->type;
        switch(type) {
            case TokenType::LPAREN:
                std::cout << "LPAREN" << std::endl;
                break;
            case TokenType::RPAREN:
                std::cout << "RPAREN" << std::endl;
                break;
            case TokenType::LBRACE:
                std::cout << "LBRACE" << std::endl;
                break;
            case TokenType::RBRACE:
                std::cout << "RBRACE" << std::endl;
                break;
            case TokenType::LBRACK:
                std::cout << "LBRACK" << std::endl;
                break;
            case TokenType::RBRACK:
                std::cout << "RBRACK" << std::endl;
                break;
            case TokenType::BECOMES:
                std::cout << "BECOMES" << std::endl;
                break;
            case TokenType::DOT:
                std::cout << "DOT" << std::endl;
                break;
            case TokenType::MINUS:
                std::cout << "MINUS" << std::endl;
                break;
            case TokenType::PLUS:
                std::cout << "PLUS" << std::endl;
                break;
            case TokenType::COMMA:
                std::cout << "COMMA" << std::endl;
                break;
            case TokenType::STAR:
                std::cout << "STAR" << std::endl;
                break;
            case TokenType::SLASH:
                std::cout << "SLASH" << std::endl;
                break;
            case TokenType::SEMI:
                std::cout << "SEMI" << std::endl;
                break;
            case TokenType::LT:
                std::cout << "LT" << std::endl;
                break;
            case TokenType::GT:
                std::cout << "GT" << std::endl;
                break;
            case TokenType::LE:
                std::cout << "LE" << std::endl;
                break;
            case TokenType::GE:
                std::cout << "GE" << std::endl;
                break;
            case TokenType::ID:
                std::cout << "ID: " + token->lexeme << std::endl;
                break;
            case TokenType::NUM:
                std::cout << "NUM: " + token->lexeme << std::endl;
                break;
            case TokenType::STRING:
                std::cout << "STRING: " + token->lexeme << std::endl;
                break;
            case TokenType::EQ:
                std::cout << "EQ" << std::endl;
                break;
            case TokenType::AND:
                std::cout << "AND" << std::endl;
                break;
            case TokenType::OR:
                std::cout << "OR" << std::endl;
                break;
            case TokenType::NOT:
                std::cout << "NOT" << std::endl;
                break;
            case TokenType::IF:
                std::cout << "IF" << std::endl;
                break;
            case TokenType::ELSE:
                std::cout << "ELSE" << std::endl;
                break;
            case TokenType::WHILE:
                std::cout << "WHILE" << std::endl;
                break;
            case TokenType::RETURN:
                std::cout << "RETURN" << std::endl;
                break;
            case TokenType::PRINT:
                std::cout << "PRINT" << std::endl;
                break;
            case TokenType::ADDR:
                std::cout << "ADDR" << std::endl;
                break;
            case TokenType::AT:
                std::cout << "AT" << std::endl;
                break;
        }
    }
};
