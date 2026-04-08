#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP

#include "Expr.hpp"
#include <string>
#include <vector>
#include <iostream>

// Notice how we inherit from the Visitor interface we generated!
class AstPrinter : public Visitor {
public:
    // This is the main function we will call to kick off the printing
    std::string print(Expr* expr) {
        // expr->accept(*this) returns a std::any. 
        // We know our printer always returns strings, so we cast it back!
        return std::any_cast<std::string>(expr->accept(*this));
    }

    std::any visitBinary(Binary& expr) override {
        return parenthesize(expr.op.lexeme, { expr.left.get(), expr.right.get() });
    }

    std::any visitGrouping(Grouping& expr) override {
        return parenthesize("group", { expr.expression.get() });
    }

    std::any visitLiteral(Literal& expr) override {
        // Because value is a std::variant, we use std::holds_alternative to check its type!
        if (std::holds_alternative<std::monostate>(expr.value)) {
            return std::string("nil");
        }
        if (std::holds_alternative<std::string>(expr.value)) {
            return std::get<std::string>(expr.value);
        }
        if (std::holds_alternative<double>(expr.value)) {
            // Convert double to string, and trim trailing zeros for clean output
            std::string str = std::to_string(std::get<double>(expr.value));
            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            if (str.back() == '.') str += "0";
            return str;
        }
        if (std::holds_alternative<bool>(expr.value)) {
            return std::get<bool>(expr.value) ? std::string("true") : std::string("false");
        }
        return std::string("");
    }

    std::any visitUnary(Unary& expr) override {
        return parenthesize(expr.op.lexeme, { expr.right.get() });
    }

private:
    // This helper method takes a name and a list of child expressions
    std::string parenthesize(const std::string& name, const std::vector<Expr*>& exprs) {
        std::string builder = "(" + name;
        
        for (Expr* expr : exprs) {
            builder += " ";
            // Here is the Double Dispatch "Ping-Pong" in action!
            // We tell the child to accept us, and it bounces back to the correct visit method.
            builder += std::any_cast<std::string>(expr->accept(*this));
        }
        
        builder += ")";
        return builder;
    }
};

#endif