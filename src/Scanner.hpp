#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <vector>
#include "Token.hpp"

class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;
    
    int start = 0;
    int current = 0;
    int line = 1;

public:
    // Constructor
    Scanner(std::string source);
    
    // The main function that will be called from main.cpp
    std::vector<Token> scanTokens();
};

#endif