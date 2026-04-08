#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "Expr.hpp"
#include <any>

// The Interpreter is just another Visitor, exactly like the AstPrinter!
class Interpreter : public Visitor {
public:
    std::any visitLiteral(Literal& expr) override;
    std::any visitGrouping(Grouping& expr) override;
    std::any visitUnary(Unary& expr) override;
    std::any visitBinary(Binary& expr) override;

private:
    // We'll add helper methods here soon
};

#endif