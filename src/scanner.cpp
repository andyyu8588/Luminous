#include "scanner.h"

#include <exception>
#include <iostream>
#include <map>

Scanner::Scanner(std::string code) { this->code = code; }

void Scanner::error(int line, std::string message) {
  std::cerr << "ERROR: " << message << " (line " << line << ")." << std::endl;
  errorOccured = true;
}

bool Scanner::isAtEnd() { return (unsigned int)current >= code.length(); }

bool Scanner::isNumber(char c) { return c >= '0' && c <= '9'; }

bool Scanner::isAlphabet(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

char Scanner::nextChar() {
  current++;
  return code.at(current - 1);
}

void Scanner::addToken(TokenType type) {
  std::string lexeme = code.substr(start, current - start);
  tokens.push_back(std::make_shared<Token>(type, lexeme, line));
}

void Scanner::addToken(TokenType type, std::string lexeme) {
  tokens.push_back(std::make_shared<Token>(type, lexeme, line));
}

bool Scanner::match(char expected) {
  if (isAtEnd() || code.at(current) != expected) return false;

  current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd()) return '\0';
  return code.at(current);
}

char Scanner::lookAhead() {
  if ((unsigned int)current + 1 >= code.length()) return '\0';
  return code.at(current + 1);
}

void Scanner::string() {
  // getting string
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      line++;
    }
    nextChar();
  }

  if (isAtEnd()) {
    error(line, "Missing end of string");
    return;
  }

  // remove closing quote symbol
  nextChar();

  std::string str = code.substr(start + 1, current - 1 - (start + 1));
  addToken(TokenType::STRING, str);
}

void Scanner::number() {
  // getting integer
  while (isNumber(peek())) {
    nextChar();
  }

  // Handling floating point numbers
  if (peek() == '.' && isNumber(lookAhead())) {
    // consume '.'
    nextChar();
    while (isNumber(peek())) {
      nextChar();
    }
  }

  // case where variable name starts with a number
  if (isAlphabet(peek())) {
    error(line, "Variable names cannot start with a number");
  }

  addToken(TokenType::NUM, code.substr(start, current - start));
}

void Scanner::id() {
  char curChar = peek();
  // checking for id
  while (isNumber(curChar) || isAlphabet(curChar)) {
    nextChar();
    curChar = peek();
  }

  std::string id = code.substr(start, current - start);
  auto typeIter = keywords.find(id);
  if (typeIter != keywords.end()) {
    addToken(typeIter->second);
  } else {
    addToken(TokenType::ID);
  }
}

void Scanner::scanToken() {
  char c = nextChar();
  switch (c) {
    case '(':
      addToken(TokenType::LPAREN);
      break;
    case ')':
      addToken(TokenType::RPAREN);
      break;
    case '{':
      addToken(TokenType::LBRACE);
      break;
    case '}':
      addToken(TokenType::RBRACE);
      break;
    case '[':
      addToken(TokenType::LBRACK);
      break;
    case ']':
      addToken(TokenType::RBRACK);
      break;
    case '=':
      addToken(TokenType::BECOMES);
      break;
    case '.':
      addToken(TokenType::DOT);
      break;
    case '-':
      addToken(TokenType::MINUS);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '*':
      addToken(TokenType::STAR);
      break;
    case ';':
      addToken(TokenType::SEMI);
      break;
    case '<':
      if (match('=')) {
        addToken(TokenType::LE);
      } else {
        addToken(TokenType::LT);
      }
      break;
    case '>':
      if (match('=')) {
        addToken(TokenType::GE);
      } else {
        addToken(TokenType::GT);
      }
      break;
    case '/':
      // Handling single line comments:
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) {
          nextChar();
        }
      } else {
        addToken(TokenType::SLASH);
      }
      break;
    // useless characters filters:
    case ' ':
      break;
    case '\r':
      break;
    case '\t':
      break;
    // newline:
    case '\n':
      line++;
      break;
    // strings:
    case '"':
      string();
      break;
    default:
      if (isNumber(c)) {
        // could be a number
        number();
      } else if (isAlphabet(c)) {
        // could be an id or keyword
        id();
      } else {
        std::string message = "Unexpected character \'";
        message.push_back(c);
        message += "\'";
        error(line, message);
        break;
      }
  }
}

void Scanner::tokenize() {
  // this loop scans token by token
  while (!isAtEnd()) {
    // next lexeme
    start = current;
    scanToken();
  }
}

void Scanner::reset(std::string code) {
  start = 0;
  current = 0;
  curToken = 0;
  line = 1;
  errorOccured = false;
  this->code = code;
  tokens.clear();
}
