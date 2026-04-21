#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.hpp"
#include "Token.hpp"
#include "AstPrinter.hpp"
#include "Parser.hpp"
#include "Expr.hpp"
#include "Interpreter.hpp"

bool hadError = false;
bool hadRuntimeError = false;

// Move the interpreter to the global scope so it retains memory between lines!
Interpreter interpreter; 

// A dedicated function to handle runtime errors
void runtimeError(const RuntimeError& error) {
    std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
    hadRuntimeError = true;
}

void report(int line, const std::string& where, const std::string& message) {
    // std::cerr prints to the standard error stream instead of standard output
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}
// For parse errors
void error(const Token& token, const std::string& message) {
    if (token.type == TokenType::END_OF_FILE) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void run(const std::string& source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    if (hadError) {
        return; 
    }
    interpreter.interpret(statements);
}

void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << "\n";
        exit(74);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());

    // Exit 65 for syntax errors
    if (hadError) exit(65); 
    // Exit 70 for runtime errors (like type mismatches)
    if (hadRuntimeError) exit(70); 
}

void runPrompt() {
    std::string line;
    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break; 
        }
        run(line);
        
        // Reset BOTH flags so the user can keep typing after a mistake
        hadError = false; 
        hadRuntimeError = false; 
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: cpplox [script]\n";
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }
    return 0;
}
