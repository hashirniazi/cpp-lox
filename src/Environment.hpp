#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <unordered_map>
#include <string>
#include <any>
#include "Token.hpp"
#include "RuntimeError.hpp"

class Environment {
private:
    std::unordered_map<std::string, std::any> values;
    Environment* enclosing; // <-- The link to the parent!

public:
    // Global scope constructor (no parent)
    Environment() : enclosing(nullptr) {}

    // Local scope constructor (has a parent)
    Environment(Environment* enclosing) : enclosing(enclosing) {}

    // Define remains exactly the same!
    void define(const std::string& name, std::any value) {
        values[name] = std::move(value);
    }

    std::any get(const Token& name) {
        auto elem = values.find(name.lexeme);
        if (elem != values.end()) {
            return elem->second;
        }

        // NEW: If we didn't find it locally, check the parent!
        if (enclosing != nullptr) {
            return enclosing->get(name);
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void assign(const Token& name, std::any value) {
        if (values.find(name.lexeme) != values.end()) {
            values[name.lexeme] = std::move(value);
            return;
        }

        // NEW: If we didn't find it locally, tell the parent to assign it!
        if (enclosing != nullptr) {
            enclosing->assign(name, std::move(value));
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
};

#endif