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

// The very top rule just kicks things off
std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

// Translates: equality -> comparison ( ( "!=" | "==" ) comparison )* ;
std::unique_ptr<Expr> Parser::equality() {
    // 1. Grab the left side
    std::unique_ptr<Expr> expr = comparison();

    // 2. Loop as long as we see a matching operator
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        
        // 3. Grab the right side
        std::unique_ptr<Expr> right = comparison();
        
        // 4. Combine them into a new Binary node, which becomes the new "left"
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

