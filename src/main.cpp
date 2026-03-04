#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.hpp"
#include "Token.hpp"

bool hadError = false;

void report(int line, const std::string& where, const std::string& message) {
    // std::cerr prints to the standard error stream instead of standard output
    std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}

void run(const std::string& source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    for (const auto& token : tokens) {
        std::cout << token.toString() << "\n";
    }
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