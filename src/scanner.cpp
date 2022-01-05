#include "scanner.h"

#include <exception>
#include <iostream>
#include <map>

#include "error.h"

#ifdef DEBUG
#include "debug.h"
#endif

Scanner::Scanner() {}

bool Scanner::isAtEnd() { return (unsigned int)current >= code->length(); }

bool Scanner::isNumber(char c) { return c >= '0' && c <= '9'; }

bool Scanner::isAlphabet(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

char Scanner::nextChar() {
  current++;
  return code->at(current - 1);
}

void Scanner::addToken(TokenType type) {
  std::string lexeme = code->substr(start, current - start);
  tokens.emplace_back(type, lexeme, line);
}

void Scanner::addToken(TokenType type, std::string lexeme) {
  tokens.emplace_back(type, lexeme, line);
}

bool Scanner::match(char expected) {
  if (isAtEnd() || code->at(current) != expected) return false;

  current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd()) return '\0';
  return code->at(current);
}

char Scanner::lookAhead() {
  if ((unsigned int)current + 1 >= code->length()) return '\0';
  return code->at(current + 1);
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

  std::string str = code->substr(start + 1, current - 1 - (start + 1));
  addToken(TOKEN_STRING, str);
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

  addToken(TOKEN_NUM, code->substr(start, current - start));
}

void Scanner::id() {
  char curChar = peek();
  // checking for id
  while (isNumber(curChar) || isAlphabet(curChar)) {
    nextChar();
    curChar = peek();
  }

  std::string id = code->substr(start, current - start);
  auto typeIter = keywords.find(id);
  if (typeIter != keywords.end()) {
    addToken(typeIter->second);
  } else {
    addToken(TOKEN_ID);
  }
}

void Scanner::scanToken() {
  char c = nextChar();
  switch (c) {
    case '(':
      addToken(TOKEN_LPAREN);
      break;
    case ')':
      addToken(TOKEN_RPAREN);
      break;
    case '{':
      addToken(TOKEN_LBRACE);
      break;
    case '}':
      addToken(TOKEN_RBRACE);
      break;
    case '[':
      addToken(TOKEN_LBRACK);
      break;
    case ']':
      addToken(TOKEN_RBRACK);
      break;
    case '=':
      addToken(TOKEN_BECOMES);
      break;
    case '.':
      addToken(TOKEN_DOT);
      break;
    case '-':
      addToken(TOKEN_MINUS);
      break;
    case '+':
      addToken(TOKEN_PLUS);
      break;
    case ',':
      addToken(TOKEN_COMMA);
      break;
    case '*':
      addToken(TOKEN_STAR);
      break;
    case ';':
      addToken(TOKEN_SEMI);
      break;
    case '<':
      if (match('=')) {
        addToken(TOKEN_LE);
      } else {
        addToken(TOKEN_LT);
      }
      break;
    case '>':
      if (match('=')) {
        addToken(TOKEN_GE);
      } else {
        addToken(TOKEN_GT);
      }
      break;
    case '/':
      // Handling single line comments:
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd()) {
          nextChar();
        }
      } else {
        addToken(TOKEN_SLASH);
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
#ifdef DEBUG
  printTokens(tokens);
#endif
}

void Scanner::reset(const std::string& code) {
  start = 0;
  current = 0;
  curToken = 0;
  line = 1;
  errorOccured = false;
  panicMode = false;
  this->code = &code;
  tokens.clear();
}

const Token& Scanner::getNextToken() {
  try {
    return tokens.at(curToken++);
  } catch (std::out_of_range) {
    tokens.emplace_back(TOKEN_EOF, "", line);
    return tokens.at(curToken - 1);
  }
}
