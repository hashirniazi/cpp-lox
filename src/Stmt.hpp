#ifndef STMT_HPP
#define STMT_HPP

#include "Expr.hpp"
#include <memory>

// Forward declarations so the classes know about each other
class Expression; 
class Print;
class Var;

// The Visitor interface for Statements (Returns void!)
class StmtVisitor {
public:
    virtual void visitExpressionStmt(Expression& stmt) = 0;
    virtual void visitPrintStmt(Print& stmt) = 0;
    virtual void visitVarStmt(Var& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// The base Statement class
class Stmt {
public:
    virtual void accept(StmtVisitor& visitor) = 0;
    virtual ~Stmt() = default;
};

class Print : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    Print(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitPrintStmt(*this);
    }
};

class Expression : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    Expression(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitExpressionStmt(*this);
    }
};

class Var : public Stmt {
public:
    Token name;
    std::unique_ptr<Expr> initializer; // Can be nullptr if uninitialized!

    Var(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) override {
        visitor.visitVarStmt(*this);
    }
};

#endif