#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <iostream>
#include "Expr.hpp"
#include "Token.hpp"   // Needed for the Token in our helper and error
#include <any>
#include <stdexcept>   // Needed for std::runtime_error
#include <string>

// Our custom runtime error that stores the token where the error happened
class RuntimeError : public std::runtime_error {
public:
    const Token token;
    
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

class Interpreter : public Visitor {
public:
    std::any visitLiteral(Literal& expr) override;
    std::any visitGrouping(Grouping& expr) override;
    std::any visitUnary(Unary& expr) override;
    std::any visitBinary(Binary& expr) override;

    void interpret(Expr* expression);

private:
    std::any evaluate(Expr* expr);
    
    // Our three new helpers!
    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);
    void checkNumberOperands(Token op, const std::any& left, const std::any& right);
    void checkNumberOperand(Token op, const std::any& operand);
    
    std::string stringify(const std::any& object);
};

#endif