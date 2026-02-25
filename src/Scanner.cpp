#include "Scanner.hpp"

// Constructor implementation
Scanner::Scanner(std::string source) : source(source) {}

std::vector<Token> Scanner::scanTokens() {
    // We will build the loop here!
    
    tokens.push_back(Token(TokenType::END_OF_FILE, "", std::monostate{}, line));
    return tokens;
}