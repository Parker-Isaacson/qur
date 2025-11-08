#ifndef AST_H
#define AST_H

#include "lexer.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

class astError : public std::exception {
private:
    std::string msg_;

public:
    astError(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override {
        return msg_.c_str();
    }
};

enum class astNodeType {
    GENERIC = 0,
        EXPRESSION,
            LITERAL,
                INT,
                DOUBLE,
                CHAR,
                BOOL,
            VARIABLE,
            UNARYOP,
            BINARYOP,
            ASSIGNOP,
            FNCALL,
        STATEMENT,
            IF,
                CONDITION,
            LOOP,
                FOR,
                WHILE,
            RETURN,
            BREAK,
            CONTINUE,
        DECLARATION,
            FUNCTION,
            VARDECL,
        BODY,
        PROGRAM,
};

// Forward declarations
class AST;

struct astNode {
    virtual ~astNode() = default;
    astNodeType type = astNodeType::GENERIC;
    virtual void print(int indent = 0) const = 0;
    virtual void generateASM() const = 0;
    virtual std::string describe() const = 0;
};

struct expressionNode : astNode {
    expressionNode() { type = astNodeType::EXPRESSION; }
};

struct literalNode : expressionNode {
    literalNode() { type = astNodeType::LITERAL; }
};

struct stringLiteralNode : literalNode {
    std::string value;
    explicit stringLiteralNode(std::string v) : value(std::move(v)) {
        type = astNodeType::LITERAL;
    }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "string(\"" << value << "\")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "STRING literal: " + value; }
};

struct intLiteralNode : literalNode {
    int value;
    explicit intLiteralNode(int v) : value(v) { type = astNodeType::INT; }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "int(" << value << ")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "INT literal: " + std::to_string(value); }
};

struct doubleLiteralNode : literalNode {
    double value;
    explicit doubleLiteralNode(double v) : value(v) { type = astNodeType::DOUBLE; }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "double(" << value << ")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "DOUBLE literal: " + std::to_string(value); }
};

struct charLiteralNode : literalNode {
    char value;
    explicit charLiteralNode(char v) : value(v) { type = astNodeType::CHAR; }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "char('" << value << "')\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return std::string("CHAR literal: '") + value + "'"; }
};

struct booleanLiteralNode : literalNode {
    bool value;
    explicit booleanLiteralNode(bool v) : value(v) { type = astNodeType::BOOL; }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "bool(" << (value ? "true" : "false") << ")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return std::string("BOOL literal: ") + (value ? "true" : "false"); }
};

enum class astVarType {
    VOID,
    INT,
    DOUBLE,
    STRING,
    CHAR,
    BOOLEAN,
    INFERRED,
};

struct variableNode : expressionNode {
    std::string name;
    astVarType varType;

    variableNode(std::string n, astVarType t = astVarType::INFERRED)
        : name(std::move(n)), varType(t) {
        type = astNodeType::VARIABLE;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Variable(\"" << name << "\", type=" << (int)varType << ")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "Variable: " + name; }
};

struct unaryOpNode : expressionNode {
    std::string op;
    std::unique_ptr<expressionNode> operand;

    unaryOpNode(std::string o, std::unique_ptr<expressionNode> expr)
        : op(std::move(o)), operand(std::move(expr)) {
        type = astNodeType::UNARYOP;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "UnaryOp(" << op << ")\n";
        if (operand) operand->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Unary operation: " + op; }
};

struct binaryOpNode : expressionNode {
    std::string op;
    std::unique_ptr<expressionNode> left;
    std::unique_ptr<expressionNode> right;

    binaryOpNode(std::string o, std::unique_ptr<expressionNode> l, std::unique_ptr<expressionNode> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {
        type = astNodeType::BINARYOP;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BinaryOp(" << op << ")\n";
        if (left) left->print(indent + 2);
        if (right) right->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Binary operation: " + op; }
};

struct assignOpNode : expressionNode {
    std::string targetName;
    std::unique_ptr<expressionNode> value;
    std::string op; // e.g. '=', '+=', '-=', etc.

    assignOpNode(std::string t, std::unique_ptr<expressionNode> v, std::string o = "=")
        : targetName(std::move(t)), value(std::move(v)), op(std::move(o)) {
        type = astNodeType::ASSIGNOP;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ')
                  << "AssignOp(target=\"" << targetName << "\", op=\"" << op << "\")\n";
        if (value) value->print(indent + 2);
    }

    void generateASM() const override {}
    std::string describe() const override { 
        return "Assignment (" + op + ") to: " + targetName; 
    }
};

struct fnCallNode : expressionNode {
    std::string name;
    std::vector<std::unique_ptr<expressionNode>> args;

    fnCallNode(std::string n, std::vector<std::unique_ptr<expressionNode>> a = {})
        : name(std::move(n)), args(std::move(a)) {
        type = astNodeType::FNCALL;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FnCall(\"" << name << "\")\n";
        for (const auto &a : args) {
            if (a) a->print(indent + 2);
        }
    }
    void generateASM() const override {}
    std::string describe() const override { return "Function call: " + name; }
};

struct statementNode : astNode {
    statementNode() { type = astNodeType::STATEMENT; }
};


struct importNode : statementNode {
    std::string path;
    explicit importNode(std::string p) : path(std::move(p)) { type = astNodeType::STATEMENT; }
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Import(" << path << ")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "Import: " + path; }
};
struct ifNode : statementNode {
    std::unique_ptr<expressionNode> condition;
    std::unique_ptr<astNode> thenBody;
    std::unique_ptr<astNode> elseBody;

    ifNode(std::unique_ptr<expressionNode> c, std::unique_ptr<astNode> t, std::unique_ptr<astNode> e = nullptr)
        : condition(std::move(c)), thenBody(std::move(t)), elseBody(std::move(e)) {
        type = astNodeType::IF;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IfStatement\n";
        std::cout << std::string(indent + 2, ' ') << "Condition:\n";
        if (condition) condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Then:\n";
        if (thenBody) thenBody->print(indent + 4);
        if (elseBody) {
            std::cout << std::string(indent + 2, ' ') << "Else:\n";
            elseBody->print(indent + 4);
        }
    }
    void generateASM() const override {}
    std::string describe() const override { return "If statement"; }
};

struct loopNode : statementNode {
    loopNode() { type = astNodeType::LOOP; }
};

struct forNode : loopNode {
    std::unique_ptr<astNode> init;
    std::unique_ptr<expressionNode> condition;
    std::unique_ptr<expressionNode> increment;
    std::unique_ptr<astNode> body;

    forNode(std::unique_ptr<astNode> i, std::unique_ptr<expressionNode> c,
            std::unique_ptr<expressionNode> inc, std::unique_ptr<astNode> b)
        : init(std::move(i)), condition(std::move(c)), increment(std::move(inc)), body(std::move(b)) {
        type = astNodeType::FOR;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ForLoop\n";
        std::cout << std::string(indent + 2, ' ') << "Init:\n";
        if (init) init->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Condition:\n";
        if (condition) condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Increment:\n";
        if (increment) increment->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Body:\n";
        if (body) body->print(indent + 4);
    }
    void generateASM() const override {}
    std::string describe() const override { return "For loop"; }
};

struct whileNode : loopNode {
    std::unique_ptr<expressionNode> condition;
    std::unique_ptr<astNode> body;

    whileNode(std::unique_ptr<expressionNode> c, std::unique_ptr<astNode> b)
        : condition(std::move(c)), body(std::move(b)) {
        type = astNodeType::WHILE;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "WhileLoop\n";
        std::cout << std::string(indent + 2, ' ') << "Condition:\n";
        if (condition) condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Body:\n";
        if (body) body->print(indent + 4);
    }
    void generateASM() const override {}
    std::string describe() const override { return "While loop"; }
};

struct returnNode : statementNode {
    std::unique_ptr<expressionNode> value;

    explicit returnNode(std::unique_ptr<expressionNode> v = nullptr)
        : value(std::move(v)) {
        type = astNodeType::RETURN;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Return\n";
        if (value) value->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Return statement"; }
};

struct breakNode : statementNode {
    breakNode() { type = astNodeType::BREAK; }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Break\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "Break statement"; }
};

struct continueNode : statementNode {
    continueNode() { type = astNodeType::CONTINUE; }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Continue\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return "Continue statement"; }
};

struct declarationNode : astNode {
    declarationNode() { type = astNodeType::DECLARATION; }
};

struct varDeclNode : declarationNode {
    astVarType varType;
    std::string name;
    std::unique_ptr<expressionNode> initializer;

    varDeclNode(astVarType t, std::string n, std::unique_ptr<expressionNode> init = nullptr)
        : varType(t), name(std::move(n)), initializer(std::move(init)) {
        type = astNodeType::VARDECL;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarDecl(\"" << name << "\", type=" << (int)varType << ")\n";
        if (initializer) {
            std::cout << std::string(indent + 2, ' ') << "Initializer:\n";
            initializer->print(indent + 4);
        }
    }
    void generateASM() const override {}
    std::string describe() const override { return "Variable declaration: " + name; }
};

struct paramNode {
    astVarType type;
    std::string name;

    paramNode(astVarType t, std::string n) : type(t), name(std::move(n)) {}
};

struct functionNode : declarationNode {
    astVarType returnType;
    std::string name;
    std::vector<paramNode> params;
    std::unique_ptr<astNode> body;

    functionNode(astVarType rt, std::string n, std::vector<paramNode> p, std::unique_ptr<astNode> b)
        : returnType(rt), name(std::move(n)), params(std::move(p)), body(std::move(b)) {
        type = astNodeType::FUNCTION;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Function(\"" << name << "\", returnType=" << (int)returnType << ")\n";
        std::cout << std::string(indent + 2, ' ') << "Params:\n";
        for (const auto &param : params) {
            std::cout << std::string(indent + 4, ' ') << "Param(\"" << param.name << "\", type=" << (int)param.type << ")\n";
        }
        if (body) {
            std::cout << std::string(indent + 2, ' ') << "Body:\n";
            body->print(indent + 4);
        }
    }
    void generateASM() const override {}
    std::string describe() const override { return "Function: " + name; }
};

struct bodyNode : astNode {
    std::vector<std::unique_ptr<astNode>> statements;

    explicit bodyNode(std::vector<std::unique_ptr<astNode>> s = {})
        : statements(std::move(s)) {
        type = astNodeType::BODY;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Body {\n";
        for (const auto &stmt : statements) {
            if (stmt) stmt->print(indent + 2);
        }
        std::cout << std::string(indent, ' ') << "}\n";
    }
    void generateASM() const override {}
    std::string describe() const override { 
        return "Body block with " + std::to_string(statements.size()) + " statement(s)"; 
    }
};

struct programNode : astNode {
    std::vector<std::unique_ptr<astNode>> declarations;

    explicit programNode(std::vector<std::unique_ptr<astNode>> decls = {})
        : declarations(std::move(decls)) {
        type = astNodeType::PROGRAM;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Program\n";
        for (const auto &decl : declarations) {
            if (decl) decl->print(indent + 2);
        }
    }
    void generateASM() const override {}
    std::string describe() const override {
        return "Program with " + std::to_string(declarations.size()) + " top-level declaration(s)";
    }
};

// Main AST class for building and managing the abstract syntax tree
class AST {
private:
    std::vector<Token> tokens_;
    size_t current_;
    std::unique_ptr<programNode> root_;

    // Helper methods
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool isAtEnd() const;
    Token consume(TokenType type, const std::string& errorMsg);

    // Type conversion
    astVarType tokenTypeToVarType(TokenType type);

    // Parsing methods
    std::unique_ptr<astNode> parseDeclaration();
    std::unique_ptr<functionNode> parseFunction();
    std::unique_ptr<varDeclNode> parseVarDeclaration();
    std::unique_ptr<astNode> parseStatement();
    std::unique_ptr<ifNode> parseIfStatement();
    std::unique_ptr<whileNode> parseWhileStatement();
    std::unique_ptr<forNode> parseForStatement();
    std::unique_ptr<returnNode> parseReturnStatement();
    std::unique_ptr<bodyNode> parseBody();
    std::unique_ptr<expressionNode> parseExpression();
    std::unique_ptr<expressionNode> parseAssignment();
    std::unique_ptr<expressionNode> parseLogicalOr();
    std::unique_ptr<expressionNode> parseLogicalAnd();
    std::unique_ptr<expressionNode> parseEquality();
    std::unique_ptr<expressionNode> parseComparison();
    std::unique_ptr<expressionNode> parseAddition();
    std::unique_ptr<expressionNode> parseMultiplication();
    std::unique_ptr<expressionNode> parseUnary();
    std::unique_ptr<expressionNode> parsePrimary();
    std::unique_ptr<expressionNode> parseCall();

public:
    explicit AST(const std::vector<Token>& tokens);
    void build();
    void print() const;
    const programNode* getRoot() const { return root_.get(); }
    void generateCode() const;
};

#endif // AST_H
