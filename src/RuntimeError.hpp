#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include <stdexcept>
#include <string>
#include "Token.hpp"

class RuntimeError : public std::runtime_error {
public:
    const Token token;
    
    RuntimeError(const Token& token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

#endif