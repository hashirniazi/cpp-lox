#ifndef Expr_HPP
#define Expr_HPP

#include <any>
#include <memory>
#include <vector>
#include "Token.hpp"

struct Binary;
struct Grouping;
struct Literal;
struct Unary;
class Variable;
class Assign;
class Logical;

struct Visitor {
    virtual ~Visitor() = default;
    virtual std::any visitBinary(Binary& expr) = 0;
    virtual std::any visitGrouping(Grouping& expr) = 0;
    virtual std::any visitLiteral(Literal& expr) = 0;
    virtual std::any visitUnary(Unary& expr) = 0;
    virtual std::any visitVariableExpr(Variable& expr) = 0;
    virtual std::any visitAssignExpr(Assign& expr) = 0;
    virtual std::any visitLogicalExpr(Logical& expr) = 0;

};

struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(Visitor& visitor) = 0;
};

struct Binary : public Expr {
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitBinary(*this);
    }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct Grouping : public Expr {
    Grouping(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitGrouping(*this);
    }

    std::unique_ptr<Expr> expression;
};

struct Literal : public Expr {
    Literal(LiteralValue value)
      : value(std::move(value)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitLiteral(*this);
    }

    LiteralValue value;
};

struct Unary : public Expr {
    Unary(Token op, std::unique_ptr<Expr> right)
      : op(std::move(op)), right(std::move(right)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitUnary(*this);
    }

    Token op;
    std::unique_ptr<Expr> right;
};

class Variable : public Expr {
public:
    Token name;

    Variable(Token name) : name(std::move(name)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitVariableExpr(*this);
    }
};

class Assign : public Expr {
public:
    Token name;
    std::unique_ptr<Expr> value;

    Assign(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitAssignExpr(*this);
    }
};

class Logical : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Logical(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(Visitor& visitor) override {
        return visitor.visitLogicalExpr(*this);
    }
};

#endif
