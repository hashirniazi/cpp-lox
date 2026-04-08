#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// Returns the token we are currently looking at
Token Parser::peek() {
    return tokens[current];
}

// Returns the most recently consumed token
Token Parser::previous() {
    return tokens[current - 1];
}

// Checks if we've run out of tokens to parse
bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

// Consumes the current token and returns it
Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

// Checks if the current token is of a specific type WITHOUT consuming it
bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

// Checks if the current token matches any of the given types. 
// If it does, it consumes the token and returns true.
bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}