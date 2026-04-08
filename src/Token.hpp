#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <variant> // Changed from <any>
#include "TokenType.hpp"

// A type alias to make our code cleaner to read
using LiteralValue = std::variant<std::monostate, std::string, double, bool>;

class Token {
public:
    const TokenType type;
    const std::string lexeme;
    const LiteralValue literal;
    const int line;

    Token(TokenType type, std::string lexeme, LiteralValue literal, int line)
        : type(type), lexeme(lexeme), literal(std::move(literal)), line(line) {}

    std::string toString() const {
        std::string literalText = "null";
        
        // Safely check and extract the value from the variant
        if (std::holds_alternative<std::string>(literal)) {
            literalText = std::get<std::string>(literal);
        } else if (std::holds_alternative<double>(literal)) {
            literalText = std::to_string(std::get<double>(literal));
        }

        return std::to_string(static_cast<int>(type)) + " " + lexeme + " " + literalText;
    }
};

#endif