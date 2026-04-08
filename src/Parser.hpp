#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include <initializer_list>
#include <stdexcept> // Needed for std::runtime_error
#include "Token.hpp"
#include "Expr.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

    // --- The Error Struct ---
    struct ParseError : public std::runtime_error {
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

    // --- Navigation Helpers ---
    Token peek();
    Token previous();
    bool isAtEnd();
    Token advance();
    bool check(TokenType type);
    bool match(std::initializer_list<TokenType> types);

    // --- Error Handling ---
    Token consume(TokenType type, std::string message);
    ParseError error(Token token, std::string message);
    void synchronize();

    // --- Grammar Rules ---
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

public:
    Parser(const std::vector<Token>& tokens);
};

#endif