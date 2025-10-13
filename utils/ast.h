#ifndef AST_H
#define AST_H

#include "lexer.h"
#include <memory>
#include <string>

// For printing, mostly debug
#include <iostream>

class astError : public std::exception {
private:
    std::string msg_; // No default error

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
                // STRING,
                CHAR,
                BOOL,
            VARIABLE,
            // LIST
            // TUPLE
            // DICT
            // INDEX
            // TERNARY
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
        DECLARATION,
            FUNCTION,
            // OPERATOR,
            // CLASS
            // STRUCT
            // ENUM
        BODY,
        // IMPORT
};

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
        std::cout << std::string(indent, ' ') << "char(" << value << ")\n";
    }
    void generateASM() const override {}
    std::string describe() const override { return std::string("CHAR literal: ") + value; }
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
    INT,
    DOUBLE,
    STRING,
    CHAR,
    BOOLEAN,
    LIST,
    TUPLE,
    DICT,
};

struct variableNode : expressionNode {
    astVarType varType;
    std::unique_ptr<literalNode> value;

    variableNode(std::unique_ptr<literalNode> v, astVarType t)
        : value(std::move(v)), varType(t) {
        type = astNodeType::VARIABLE;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Variable(" << (int)varType << "): ";
        if (value) value->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Variable of type " + std::to_string((int)varType); }
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
    std::unique_ptr<variableNode> target;
    std::unique_ptr<expressionNode> value;

    assignOpNode(std::unique_ptr<variableNode> t, std::unique_ptr<expressionNode> v)
        : target(std::move(t)), value(std::move(v)) {
        type = astNodeType::ASSIGNOP;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "AssignOp\n";
        if (target) target->print(indent + 2);
        if (value) value->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Assignment operation"; }
};

struct fnCallNode : expressionNode {
    std::string name;
    std::vector<std::unique_ptr<expressionNode>> args;

    fnCallNode(std::string n, std::vector<std::unique_ptr<expressionNode>> a)
        : name(std::move(n)), args(std::move(a)) {
        type = astNodeType::FNCALL;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FnCall(" << name << ")\n";
        for (auto &a : args) a->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Function call: " + name; }
};

struct statementNode : astNode {
    statementNode() { type = astNodeType::STATEMENT; }
};

struct ifNode : statementNode {
    std::unique_ptr<astNode> condition;
    std::unique_ptr<astNode> body;

    ifNode(std::unique_ptr<astNode> c, std::unique_ptr<astNode> b)
        : condition(std::move(c)), body(std::move(b)) {
        type = astNodeType::IF;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "IfStatement\n";
        if (condition) condition->print(indent + 2);
        if (body) body->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "If statement"; }
};

struct conditionNode : statementNode {
    std::unique_ptr<expressionNode> expr;
    explicit conditionNode(std::unique_ptr<expressionNode> e)
        : expr(std::move(e)) {
        type = astNodeType::CONDITION;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Condition\n";
        if (expr) expr->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Condition node"; }
};

struct loopNode : statementNode {
    loopNode() { type = astNodeType::LOOP; }
};

struct forNode : loopNode {
    std::unique_ptr<astNode> init;
    std::unique_ptr<astNode> cond;
    std::unique_ptr<astNode> incr;
    std::unique_ptr<astNode> body;

    forNode(std::unique_ptr<astNode> i, std::unique_ptr<astNode> c,
            std::unique_ptr<astNode> inc, std::unique_ptr<astNode> b)
        : init(std::move(i)), cond(std::move(c)), incr(std::move(inc)), body(std::move(b)) {
        type = astNodeType::FOR;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ForLoop\n";
        if (init) init->print(indent + 2);
        if (cond) cond->print(indent + 2);
        if (incr) incr->print(indent + 2);
        if (body) body->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "For loop"; }
};

struct whileNode : loopNode {
    std::unique_ptr<astNode> cond;
    std::unique_ptr<astNode> body;

    whileNode(std::unique_ptr<astNode> c, std::unique_ptr<astNode> b)
        : cond(std::move(c)), body(std::move(b)) {
        type = astNodeType::WHILE;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "WhileLoop\n";
        if (cond) cond->print(indent + 2);
        if (body) body->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "While loop"; }
};

struct returnNode : statementNode {
    std::unique_ptr<expressionNode> value;

    explicit returnNode(std::unique_ptr<expressionNode> v)
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

struct declarationNode : astNode {
    declarationNode() { type = astNodeType::DECLARATION; }
};

struct functionNode : declarationNode {
    std::string name;
    std::vector<std::unique_ptr<variableNode>> params;
    std::unique_ptr<astNode> body;

    functionNode(std::string n, std::vector<std::unique_ptr<variableNode>> p, std::unique_ptr<astNode> b)
        : name(std::move(n)), params(std::move(p)), body(std::move(b)) {
        type = astNodeType::FUNCTION;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Function(" << name << ")\n";
        std::cout << std::string(indent + 2, ' ') << "Params:\n";
        for (auto &param : params) param->print(indent + 4);
        if (body) {
            std::cout << std::string(indent + 2, ' ') << "Body:\n";
            body->print(indent + 4);
        }
    }
    void generateASM() const override {}
    std::string describe() const override { return "Function declaration: " + name; }
};

struct bodyNode : astNode {
    std::vector<std::unique_ptr<astNode>> statements;

    explicit bodyNode(std::vector<std::unique_ptr<astNode>> s = {})
        : statements(std::move(s)) {
        type = astNodeType::BODY;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Body\n";
        for (auto &stmt : statements) stmt->print(indent + 2);
    }
    void generateASM() const override {}
    std::string describe() const override { return "Body block"; }
};
#endif // AST_H
