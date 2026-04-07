#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "Token.hpp"

class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;
    
    int start = 0;
    int current = 0;
    int line = 1;

    // Helpers we will implement in the .cpp file
    bool match(char expected);
    void scanToken();
    void addToken(TokenType type);
    void addToken(TokenType type, LiteralValue literal);
    char advance();
    void string();
    char peekNext();
    void number();
    void identifier();
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    const std::unordered_map<std::string, TokenType> keywords = {
        {"and",    TokenType::AND},
        {"class",  TokenType::CLASS},
        {"else",   TokenType::ELSE},
        {"false",  TokenType::FALSE},
        {"for",    TokenType::FOR},
        {"fun",    TokenType::FUN},
        {"if",     TokenType::IF},
        {"nil",    TokenType::NIL},
        {"or",     TokenType::OR},
        {"print",  TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super",  TokenType::SUPER},
        {"this",   TokenType::THIS},
        {"true",   TokenType::TRUE},
        {"var",    TokenType::VAR},
        {"while",  TokenType::WHILE}
    };

    bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

    bool isAtEnd() {
    return current >= source.length();
}

char peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

public:
    // Constructor
    Scanner(std::string source);
    
    // The main function that will be called from main.cpp
    std::vector<Token> scanTokens();
};

#endif