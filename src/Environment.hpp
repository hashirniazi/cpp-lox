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

public:
    // Adds a new variable to the map
    void define(const std::string& name, std::any value) {
        values[name] = std::move(value);
    }

    // Looks up a variable by its Token name
    std::any get(const Token& name) {
        auto elem = values.find(name.lexeme);
        if (elem != values.end()) {
            return elem->second;
        }

        // If the variable doesn't exist, we crash the program!
        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    // Updates an existing variable
    void assign(const Token& name, std::any value) {
        if (values.find(name.lexeme) != values.end()) {
            values[name.lexeme] = std::move(value);
            return;
        }

        // If it doesn't exist, we refuse to create it and crash!
        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
    
};

#endif