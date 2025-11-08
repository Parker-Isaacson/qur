#include "ast.h"
#include "lexer.h"

// Constructor
AST::AST(const std::vector<Token>& tokens)
    : tokens_(tokens), current_(0), root_(nullptr) {}

// Helper methods
Token AST::peek() const {
    if (isAtEnd()) {
        // Return a dummy EOF token instead of accessing invalid memory
        static Token eofToken = {TokenType::UNKNOWN, "", -1, -1};
        return eofToken;
    }
    return tokens_[current_];
}

Token AST::previous() const {
    if (current_ == 0) {
        if (tokens_.empty()) {
            static Token dummy = {TokenType::UNKNOWN, "", -1, -1};
            return dummy;
        }
        return tokens_[0];
    }
    return tokens_[current_ - 1];
}

Token AST::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool AST::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool AST::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool AST::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool AST::isAtEnd() const {
    return current_ >= tokens_.size();
}

Token AST::consume(TokenType type, const std::string& errorMsg) {
    if (check(type)) return advance();
    
    Token current = peek();
    std::string fullMsg = errorMsg;
    if (current.line >= 0) {
        fullMsg += " at line " + std::to_string(current.line) + ", column " + std::to_string(current.column);
    }
    fullMsg += " (found '" + current.lexme + "')";
    throw astError(fullMsg);
}

astVarType AST::tokenTypeToVarType(TokenType type) {
    switch (type) {
        case TokenType::INT: return astVarType::INT;
        case TokenType::DOUBLE: return astVarType::DOUBLE;
        case TokenType::CHAR: return astVarType::CHAR;
        case TokenType::BOOLEAN: return astVarType::BOOLEAN;
        case TokenType::STRING: return astVarType::STRING;
        case TokenType::VOID: return astVarType::VOID;
        default: return astVarType::INFERRED;
    }
}

// Main build method
void AST::build() {
    if (tokens_.empty()) {
        throw astError("No tokens to parse - input file may be empty");
    }

    std::vector<std::unique_ptr<astNode>> declarations;
    bool hadError = false;
    
    while (!isAtEnd()) {
        if (check(TokenType::SEMICOLON) || check(TokenType::RBRACE)) {
            advance();
            continue;
        }

        try {
            auto decl = parseDeclaration();
            if (decl) {
                declarations.push_back(std::move(decl));
            }
        } catch (const astError& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            hadError = true;
            
            // Synchronize: skip to next safe point
            while (!isAtEnd()) {
                Token t = peek();
                // Stop at statement/declaration boundaries
                if (t.type == TokenType::SEMICOLON) {
                    advance();
                    break;
                }
                if (t.type == TokenType::RBRACE || 
                    t.type == TokenType::FUNCTION ||
                    t.type == TokenType::IF ||
                    t.type == TokenType::WHILE ||
                    t.type == TokenType::FOR ||
                    t.type == TokenType::RETURN) {
                    break;
                }
                advance();
            }
        }
    }
    
    if (hadError) {
        throw astError("Failed to build AST due to parse errors");
    }
    
    root_ = std::make_unique<programNode>(std::move(declarations));
}

// Parse declaration (function or variable)
std::unique_ptr<astNode> AST::parseDeclaration() {
    if (check(TokenType::RBRACE) || check(TokenType::SEMICOLON)) {
        advance();
        return nullptr;
    }
    // Function declaration: fn name(...) { ... }
    if (match(TokenType::FUNCTION)) {
        return parseFunction();
    }
    
    // Import declaration: import ...;
    if (match(TokenType::IMPORT)) {
        // Accumulate tokens until semicolon as path
        std::string path;
        while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
            Token t = advance();
            path += t.lexme;
        }
        consume(TokenType::SEMICOLON, "Expected ';' after import");
        return std::make_unique<importNode>(path);
    }

    // Variable declaration: type name = expr;
    if (check(TokenType::INT) || check(TokenType::DOUBLE) || 
        check(TokenType::CHAR) || check(TokenType::BOOLEAN) || 
        check(TokenType::STRING)) {
        advance();
        return parseVarDeclaration();
    }
    
    // Otherwise it's a statement
    return parseStatement();
}

// Parse function declaration
std::unique_ptr<functionNode> AST::parseFunction() {
    // Return type (optional, defaults to void)
    astVarType returnType = astVarType::VOID;
    if (match({TokenType::INT, TokenType::DOUBLE, TokenType::CHAR, 
               TokenType::BOOLEAN, TokenType::STRING, TokenType::VOID})) {
        returnType = tokenTypeToVarType(previous().type);
    }
    
    // Function name
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected function name");
    std::string name = nameToken.lexme;
    
    // Parameters
    consume(TokenType::LPAREN, "Expected '(' after function name");
    std::vector<paramNode> params;
    
    if (!check(TokenType::RPAREN)) {
        do {
            // Parameter type
            if (!match({TokenType::INT, TokenType::DOUBLE, TokenType::CHAR, 
                       TokenType::BOOLEAN, TokenType::STRING})) {
                throw astError("Expected parameter type");
            }
            astVarType paramType = tokenTypeToVarType(previous().type);
            
            // Parameter name
            Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
            params.emplace_back(paramType, paramName.lexme);
            
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    // Function body
    auto body = parseBody();
    
    return std::make_unique<functionNode>(returnType, name, std::move(params), std::move(body));
}

// Parse variable declaration
std::unique_ptr<varDeclNode> AST::parseVarDeclaration() {
    Token typeToken = previous();
    astVarType varType = tokenTypeToVarType(typeToken.type);
    
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name");
    std::string name = nameToken.lexme;
    
    std::unique_ptr<expressionNode> initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<varDeclNode>(varType, name, std::move(initializer));
}

// Parse statement
std::unique_ptr<astNode> AST::parseStatement() {
    if (match(TokenType::IF)) {
        return parseIfStatement();
    }
    
    if (match(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    
    if (match(TokenType::FOR)) {
        return parseForStatement();
    }
    
    if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    
    if (match(TokenType::BREAK)) {
        consume(TokenType::SEMICOLON, "Expected ';' after break");
        return std::make_unique<breakNode>();
    }
    
    if (match(TokenType::CONTINUE)) {
        consume(TokenType::SEMICOLON, "Expected ';' after continue");
        return std::make_unique<continueNode>();
    }
    
    if (check(TokenType::LBRACE)) {
        auto body = parseBody();
        return body;
    }
    
    // Expression statement
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return expr;
}

// Parse if statement
std::unique_ptr<ifNode> AST::parseIfStatement() {
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto thenBranch = parseStatement();
    std::unique_ptr<astNode> elseBranch = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseBranch = parseStatement();
    }
    
    return std::make_unique<ifNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

// Parse while statement
std::unique_ptr<whileNode> AST::parseWhileStatement() {
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    auto body = parseBody();
    consume(TokenType::SEMICOLON, "Expected ';' after while body");
    return std::make_unique<whileNode>(std::move(condition), std::move(body));
}

// Parse for statement
std::unique_ptr<forNode> AST::parseForStatement() {
    consume(TokenType::LPAREN, "Expected '(' after 'for'");

    // Initializer
    std::unique_ptr<astNode> initializer;
    if (match(TokenType::SEMICOLON)) {
        initializer = nullptr;
    } else if (match({TokenType::INT, TokenType::DOUBLE, TokenType::CHAR, 
                      TokenType::BOOLEAN, TokenType::STRING})) {
        initializer = parseVarDeclaration();
        // parseVarDeclaration already consumed the semicolon
    } else {
        auto expr = parseExpression();
        consume(TokenType::SEMICOLON, "Expected ';' after for initializer");
        initializer = std::move(expr);
    }

    // Condition
    std::unique_ptr<expressionNode> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for condition");

    // Increment
    std::unique_ptr<expressionNode> increment = nullptr;
    if (!check(TokenType::RPAREN)) {
        increment = parseExpression();
    }
    consume(TokenType::RPAREN, "Expected ')' after for clauses");

    auto body = parseBody();
    consume(TokenType::SEMICOLON, "Expected ';' after for body");

    return std::make_unique<forNode>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

// Parse return statement
std::unique_ptr<returnNode> AST::parseReturnStatement() {
    std::unique_ptr<expressionNode> value = nullptr;
    
    if (!check(TokenType::SEMICOLON)) {
        value = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<returnNode>(std::move(value));
}

// Parse body (block of statements)
std::unique_ptr<bodyNode> AST::parseBody() {
    consume(TokenType::LBRACE, "Expected '{'");
    
    std::vector<std::unique_ptr<astNode>> statements;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (match(TokenType::SEMICOLON)) continue;
        auto stmt = parseDeclaration();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return std::make_unique<bodyNode>(std::move(statements));
}

// Parse expression (entry point for expression parsing)
std::unique_ptr<expressionNode> AST::parseExpression() {
    return parseAssignment();
}

// Parse assignment
std::unique_ptr<expressionNode> AST::parseAssignment() {
    auto expr = parseLogicalOr();
    
    if (match({TokenType::ASSIGN, TokenType::ASSIGN_ADD, TokenType::ASSIGN_SUB,
                TokenType::ASSIGN_MUL, TokenType::ASSIGN_DIV, TokenType::ASSIGN_MOD})) {
        std::string op = previous().lexme;
        auto value = parseAssignment();
        if (expr->type == astNodeType::VARIABLE) {
            variableNode* varNode = dynamic_cast<variableNode*>(expr.get());
            std::string name = varNode->name;
            return std::make_unique<assignOpNode>(name, std::move(value), op);
        }
        throw astError("Invalid assignment target");
    }

    return expr;
}

// Parse logical OR
std::unique_ptr<expressionNode> AST::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::OR)) {
        std::string op = previous().lexme;
        auto right = parseLogicalAnd();
        expr = std::make_unique<binaryOpNode>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

// Parse logical AND
std::unique_ptr<expressionNode> AST::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::AND)) {
        std::string op = previous().lexme;
        auto right = parseEquality();
        expr = std::make_unique<binaryOpNode>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

// Parse equality (== and !=)
std::unique_ptr<expressionNode> AST::parseEquality() {
    auto expr = parseComparison();
    
    while (match({TokenType::EQUAL, TokenType::NOTEQUAL})) {
        std::string op = previous().lexme;
        auto right = parseComparison();
        expr = std::make_unique<binaryOpNode>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

// Parse comparison (<, >, <=, >=)
std::unique_ptr<expressionNode> AST::parseComparison() {
    auto expr = parseAddition();
    
    while (match({TokenType::LESSTHAN, TokenType::MORETHAN, 
                  TokenType::LESSTHANEQUAL, TokenType::MORETHANEQUAL})) {
        std::string op = previous().lexme;
        auto right = parseAddition();
        expr = std::make_unique<binaryOpNode>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

// Parse addition and subtraction
std::unique_ptr<expressionNode> AST::parseAddition() {
    auto expr = parseMultiplication();
    
    while (match({TokenType::ADD, TokenType::SUB})) {
        std::string op = previous().lexme;
        auto right = parseMultiplication();
        expr = std::make_unique<binaryOpNode>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

// Parse multiplication, division, and modulo
std::unique_ptr<expressionNode> AST::parseMultiplication() {
    auto expr = parseUnary();
    
    while (match({TokenType::MUL, TokenType::DIV, TokenType::MOD})) {
        std::string op = previous().lexme;
        auto right = parseUnary();
        expr = std::make_unique<binaryOpNode>(op, std::move(expr), std::move(right));
    }
    
    return expr;
}

// Parse unary expressions (!, -, ~)
std::unique_ptr<expressionNode> AST::parseUnary() {
    if (match({TokenType::NOT, TokenType::SUB, TokenType::INVERT,
                TokenType::INCREMENT, TokenType::DECREMENT})) {
        std::string op = previous().lexme;
        auto right = parseUnary();
        return std::make_unique<unaryOpNode>(op, std::move(right));
    }

    return parseCall();
}

// Parse function calls and primary expressions
std::unique_ptr<expressionNode> AST::parseCall() {
    auto expr = parsePrimary();
    
    // Function call
    if (check(TokenType::LPAREN) && expr->type == astNodeType::VARIABLE) {
        advance(); // consume '('
        
        variableNode* varNode = dynamic_cast<variableNode*>(expr.get());
        std::string funcName = varNode->name;
        
        std::vector<std::unique_ptr<expressionNode>> args;
        
        if (!check(TokenType::RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(TokenType::COMMA));
        }
        
        consume(TokenType::RPAREN, "Expected ')' after function arguments");
        
        return std::make_unique<fnCallNode>(funcName, std::move(args));
    }
    
    while (match({TokenType::INCREMENT, TokenType::DECREMENT})) {
        std::string op = previous().lexme;
        expr = std::make_unique<unaryOpNode>(op + "_postfix", std::move(expr));
    }
    
    return expr;
}

// Parse primary expressions (literals, variables, grouped expressions)
std::unique_ptr<expressionNode> AST::parsePrimary() {
    // String literal
    if (match(TokenType::STRING)) {
        std::string value = previous().lexme;
        return std::make_unique<stringLiteralNode>(value);
    }

    // Boolean literals
    if (check(TokenType::IDENTIFIER)) {
        Token t = peek();
        if (t.lexme == "true" || t.lexme == "false") {
            advance();
            bool value = (t.lexme == "true");
            return std::make_unique<booleanLiteralNode>(value);
        }
    }
    
    // Number literal
    if (match(TokenType::LITERAL)) {
        std::string value = previous().lexme;
        
        // Check if it's a double (contains '.')
        if (value.find('.') != std::string::npos) {
            return std::make_unique<doubleLiteralNode>(std::stod(value));
        } else {
            return std::make_unique<intLiteralNode>(std::stoi(value));
        }
    }
    
    // Character literal
    if (match(TokenType::CHAR)) {
        std::string charStr = previous().lexme;
        char value = charStr.empty() ? '\0' : charStr[0];
        
        // Handle escape sequences
        if (charStr.length() >= 2 && charStr[0] == '\\') {
            switch (charStr[1]) {
                case 'n': value = '\n'; break;
                case 't': value = '\t'; break;
                case 'r': value = '\r'; break;
                case '0': value = '\0'; break;
                case '\\': value = '\\'; break;
                case '\'': value = '\''; break;
                default: value = charStr[1]; break;
            }
        }
        
        return std::make_unique<charLiteralNode>(value);
    }
    
    // Variable reference
    if (match(TokenType::IDENTIFIER)) {
        std::string name = previous().lexme;
        return std::make_unique<variableNode>(name);
    }
    
    // Grouped expression
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    Token current = peek();
    throw astError("Expected expression at line " + std::to_string(current.line) + 
                   ", column " + std::to_string(current.column) + 
                   " (found '" + current.lexme + "')");
}

// Print the AST
void AST::print() const {
    if (root_) {
        std::cout << "=== Abstract Syntax Tree ===\n";
        root_->print(0);
        std::cout << "============================\n";
    } else {
        std::cout << "AST is empty (not built yet)\n";
    }
}

// Generate code (placeholder for now)
void AST::generateCode() const {
    if (root_) {
        std::cout << "=== Code Generation ===\n";
        root_->generateASM();
        std::cout << "=======================\n";
    } else {
        std::cout << "Cannot generate code: AST is empty\n";
    }
}
