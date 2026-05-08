#include "Parser.hpp"

extern void error(const Token& token, const std::string& message);

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// Returns the token we are currently looking at
Token Parser::peek() {
    return tokens[current];
}

// Returns the most recently consumed token
Token Parser::previous() {
    return tokens[current - 1];
}

// Checks if we've run out of tokens to parse
bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

// Consumes the current token and returns it
Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

// Checks if the current token is of a specific type WITHOUT consuming it
bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

// Checks if the current token matches any of the given types. 
// If it does, it consumes the token and returns true.
bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

// Throws a custom ParseError if the token doesn't match
Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();

    throw error(peek(), message);
}

// Creates an error and reports it to our main Lox class
Parser::ParseError Parser::error(Token token, std::string message) {
    // We will assume you have an 'error' function in your main program 
    // that prints to the console. Bob uses this to track if the code has errors.
    ::error(token, message); 
    
    return ParseError(message);
}

void Parser::synchronize() {
    // Skip the token that caused the error
    advance();

    // Keep discarding tokens until we find a safe boundary
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return; // Safe boundary found!
            default:
                break; // Keep discarding...
        }

        advance();
    }
}

// The very top rule just kicks things off
std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = logic_or();

    // 2. If we find an '=' sign, we know it's actually an assignment!
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        
        // 3. Recursively call assignment() to get the right side (for a = b = 5)
        std::unique_ptr<Expr> value = assignment();

        // 4. Check if the left side was a valid variable name
        if (Variable* v = dynamic_cast<Variable*>(expr.get())) {
            Token name = v->name;
            return std::make_unique<Assign>(std::move(name), std::move(value));
        }

        // 5. If it wasn't a variable, throw an error!
        error(equals, "Invalid assignment target.");
    }

    return expr;
}

// Translates: equality -> comparison ( ( "!=" | "==" ) comparison )* ;
std::unique_ptr<Expr> Parser::equality() {
    // 1. Grab the left side
    std::unique_ptr<Expr> expr = comparison();

    // 2. Loop as long as we see a matching operator
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        
        // 3. Grab the right side
        std::unique_ptr<Expr> right = comparison();
        
        // 4. Combine them into a new Binary node, which becomes the new "left"
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    // If we see a ! or -
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary(); // Recursive call!
        return std::make_unique<Unary>(std::move(op), std::move(right));
    }

    // Otherwise, it must be a primary expression
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<Literal>(false);
    if (match({TokenType::TRUE})) return std::make_unique<Literal>(true);
    if (match({TokenType::NIL})) return std::make_unique<Literal>(std::monostate{});

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<Literal>(previous().literal);
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        
        // We must find a closing parenthesis, or it's a syntax error!
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        
        return std::make_unique<Grouping>(std::move(expr));
    }
    if (match({TokenType::IDENTIFIER})) {
    return std::make_unique<Variable>(previous());
    }

    // If we get here, the token doesn't start any known expression!
    throw error(peek(), "Expect expression.");
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }

    return statements;
}

std::unique_ptr<Stmt> Parser::printStatement() {
    // 1. Grab the math/text
    std::unique_ptr<Expr> value = expression();
    
    // 2. Demand the semicolon
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    
    // 3. Package it all up in our new Print node
    return std::make_unique<Print>(std::move(value));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    // 1. Grab the math/text
    std::unique_ptr<Expr> expr = expression();
    
    // 2. Demand the semicolon
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    
    // 3. Package it up in our new Expression node
    return std::make_unique<Expression>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<Block>(block());
    
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    // 1. Grab the variable name
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    // 2. Check for an initializer
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    // 3. Demand the semicolon
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    
    // 4. Return the new AST node!
    return std::make_unique<Var>(std::move(name), std::move(initializer));
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::VAR})) return varDeclaration();
        return statement();
    } catch (ParseError error) {
        synchronize();
        return nullptr;
    }
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;

    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_unique<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Expr> Parser::logic_or() {
    // 1. Grab the left side by looking exactly one step down the ladder
    std::unique_ptr<Expr> expr = logic_and();

    // 2. Loop as long as we see 'or' operators
    while (match({TokenType::OR})) {
        Token op = previous();
        
        // 3. Grab the right side by looking down the ladder again!
        std::unique_ptr<Expr> right = logic_and();
        
        // 4. Bundle it all into our new Logical AST node
        expr = std::make_unique<Logical>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logic_and() {
    // 1. Grab the left side by looking exactly one step down the ladder
    std::unique_ptr<Expr> expr = equality();

    // 2. Loop as long as we see 'and' operators
    while (match({TokenType::AND})) {
        Token op = previous();
        
        // 3. Grab the right side by looking down the ladder again
        std::unique_ptr<Expr> right = equality();
        
        // 4. Bundle it into our Logical AST node
        expr = std::make_unique<Logical>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    
    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<While>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    // 1. Parse the Initializer
    std::unique_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    // 2. Parse the Condition
    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    // 3. Parse the Increment
    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    // 4. Parse the Body
    std::unique_ptr<Stmt> body = statement();

    // --- DESUGARING START ---

    // Step A: If there is an increment, append it to the end of the body.
    if (increment != nullptr) {
        std::vector<std::unique_ptr<Stmt>> statements;
        statements.push_back(std::move(body));
        statements.push_back(std::make_unique<Expression>(std::move(increment)));
        body = std::make_unique<Block>(std::move(statements));
    }

    // Step B: Take the condition and the body and build a standard While loop.
    if (condition == nullptr) {
        // If the user left the condition blank (e.g., for(;;)), default to true!
        condition = std::make_unique<Literal>(true);
    }
    body = std::make_unique<While>(std::move(condition), std::move(body));

    // Step C: If there is an initializer, wrap the initializer and the While loop in one final Block.
    if (initializer != nullptr) {
        std::vector<std::unique_ptr<Stmt>> statements;
        statements.push_back(std::move(initializer));
        statements.push_back(std::move(body));
        body = std::make_unique<Block>(std::move(statements));
    }

    // Return the fully desugared AST node!
    return body;
}