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
    // 1. Scan the text into tokens
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    // 2. Parse the tokens into a syntax tree
    Parser parser(tokens);
    std::unique_ptr<Expr> expression = parser.parse();

    // 3. Stop if there was a syntax error
    // (If parse() panics and fails, it returns a nullptr)
    if (hadError || expression == nullptr) {
        return; 
    }

    // 4. Print the tree!
    Interpreter interpreter;
    interpreter.interpret(expression.get());
}

void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << "\n";
        exit(74); // EX_IOERR
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());

    // Indicate an error in the exit code.
    if (hadError) exit(65); // EX_DATAERR
}

void runPrompt() {
    std::string line;
    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break; // Exit loop on EOF (Ctrl+D)
        }
        run(line);
        hadError = false; // Reset the error flag in the interactive loop
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
