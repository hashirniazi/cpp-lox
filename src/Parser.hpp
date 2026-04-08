#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <memory>
#include "Token.hpp"
#include "Expr.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

public:
    // Constructor takes the list of tokens from the Scanner
    Parser(const std::vector<Token>& tokens);
};

#endif