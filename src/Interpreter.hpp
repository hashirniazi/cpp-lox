#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <iostream>
#include <vector>      // Needed for std::vector
#include <memory>      // Needed for std::unique_ptr
#include <any>
#include <stdexcept>   
#include <string>

#include "Expr.hpp"
#include "Stmt.hpp"    // We MUST include our new Statement file!
#include "Token.hpp"   

// Our custom runtime error that stores the token where the error happened
class RuntimeError : public std::runtime_error {
public:
    const Token token;
    
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

// MULTIPLE INHERITANCE: We act as both an Expression Visitor AND a Statement Visitor
class Interpreter : public Visitor, public StmtVisitor {
public:
    // --- Expression Visitor Methods ---
    std::any visitLiteral(Literal& expr) override;
    std::any visitGrouping(Grouping& expr) override;
    std::any visitUnary(Unary& expr) override;
    std::any visitBinary(Binary& expr) override;

    // --- Statement Visitor Methods ---
    void visitPrintStmt(Print& stmt) override; 
    void visitExpressionStmt(Expression& stmt) override;

    void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);

private:
    std::any evaluate(Expr* expr);
    void execute(Stmt* stmt);

    // Our helpers!
    bool isTruthy(const std::any& object);
    bool isEqual(const std::any& a, const std::any& b);
    void checkNumberOperands(Token op, const std::any& left, const std::any& right);
    void checkNumberOperand(Token op, const std::any& operand);
    
    std::string stringify(const std::any& object);
};

#endif