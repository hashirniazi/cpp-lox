#include "Interpreter.hpp"
extern void runtimeError(const RuntimeError& error);

std::any Interpreter::visitLiteral(Literal& expr) {
    if (std::holds_alternative<double>(expr.value)) {
        return std::get<double>(expr.value);
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value);
    }
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    }
    
    // If it's std::monostate (nil), we return an empty std::any
    return std::any(); 
}

void Interpreter::checkNumberOperand(Token op, const std::any& operand) {
    if (operand.type() == typeid(double)) return;
    
    // If it's not a number, throw our safe Lox error!
    throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::checkNumberOperands(Token op, const std::any& left, const std::any& right) {
    // If they are both numbers, we are good to go! Just return early.
    if (left.type() == typeid(double) && right.type() == typeid(double)) {
        return; 
    }
    
    // If we make it here, one or both of them aren't numbers. Panic!
    throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::stringify(const std::any& object) {
    // 1. Handle nil
    if (!object.has_value()) return "nil";

    // 2. Handle numbers
    if (object.type() == typeid(double)) {
        std::string text = std::to_string(std::any_cast<double>(object));
        
        // Lox prints integers without the decimal (e.g., "5" instead of "5.000000")
        // This strips the trailing zeros and the decimal point if it's a whole number.
        text.erase(text.find_last_not_of('0') + 1, std::string::npos);
        if (text.back() == '.') {
            text.pop_back();
        }
        return text;
    }

    // 3. Handle booleans
    if (object.type() == typeid(bool)) {
        return std::any_cast<bool>(object) ? "true" : "false";
    }

    // 4. Handle strings
    if (object.type() == typeid(std::string)) {
        return std::any_cast<std::string>(object);
    }

    return "unknown";
}

bool Interpreter::isEqual(const std::any& a, const std::any& b) {
    // 1. If both are nil, they are equal.
    if (!a.has_value() && !b.has_value()) return true;

    // 2. If only one is nil, they are not equal.
    if (!a.has_value() || !b.has_value()) return false;

    // 3. If they are different types, they are not equal.
    if (a.type() != b.type()) return false;

    // 4. They are the same type, so safely cast and compare them.
    if (a.type() == typeid(double)) {
        return std::any_cast<double>(a) == std::any_cast<double>(b);
    }
    if (a.type() == typeid(std::string)) {
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    }
    if (a.type() == typeid(bool)) {
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    }

    return false;
}

// This helper just sends the visitor back into the tree
std::any Interpreter::evaluate(Expr* expr) {
    return expr->accept(*this);
}

// Kick off Visitor for Statements
void Interpreter::execute(Stmt* stmt) {
    stmt->accept(*this);
}

// Grouping just evaluates whatever is inside the parentheses
std::any Interpreter::visitGrouping(Grouping& expr) {
    return evaluate(expr.expression.get());
}

std::any Interpreter::visitUnary(Unary& expr) {
    // 1. Evaluate the right side recursively
    std::any right = evaluate(expr.right.get());

    // 2. Apply the operator
    switch (expr.op.type) {
        case TokenType::MINUS:
            checkNumberOperand(expr.op, right);
            // Cast the std::any back to a double before making it negative
            return -std::any_cast<double>(right);
        case TokenType::BANG:
            // We need a helper to determine if the value is "truthy"
            return !isTruthy(right);
        default:
            break;
    }

    return std::any(); // Unreachable
}

bool Interpreter::isTruthy(const std::any& object) {
    // Condition 1: nil is falsey
    if (!object.has_value()) return false;

    // Condition 2: false is falsey
    if (object.type() == typeid(bool)) return std::any_cast<bool>(object);

    // Condition 3: Everything else is truthy!
    return true;
}

std::any Interpreter::visitBinary(Binary& expr) {
    std::any left = evaluate(expr.left.get());
    std::any right = evaluate(expr.right.get());

    switch (expr.op.type) {
        // --- Math Operators ---
        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) - std::any_cast<double>(right);
        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) / std::any_cast<double>(right);
        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) * std::any_cast<double>(right);
            
        case TokenType::PLUS:
            if (left.type() == typeid(double) && right.type() == typeid(double)) {
                return std::any_cast<double>(left) + std::any_cast<double>(right);
            }
            if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
                return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
            }
            throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
            
        // --- Comparison Operators ---
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) > std::any_cast<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) >= std::any_cast<double>(right);
        case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) < std::any_cast<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return std::any_cast<double>(left) <= std::any_cast<double>(right);

        // --- Equality Operators ---
        case TokenType::EQUAL_EQUAL:
            return isEqual(left, right);
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
    }

    return std::any(); // Unreachable
}

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            execute(statement.get());
        }
    } catch (const RuntimeError& error) {
        runtimeError(error);
    }
}

void Interpreter::visitPrintStmt(Print& stmt) {
    // 1. Evaluate the inner expression to get the raw value
    std::any value = evaluate(stmt.expression.get());
    
    // 2. Convert that value to a string and print it to the console
    std::cout << stringify(value) << "\n";
}

void Interpreter::visitExpressionStmt(Expression& stmt) {
    // We evaluate the expression so any side-effects (like math or future function calls) happen.
    // Notice we do NOT save the returned std::any value. It immediately falls out of 
    // scope and C++ automatically destroys it for us!
    evaluate(stmt.expression.get());
}