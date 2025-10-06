#ifndef AST_H
#define AST_H

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
   GENERIC,
   VARIABLE,
   BINOP,
   STRING,
   FLOAT,
   NUMBER,
};

struct astNode { // Interface
    virtual ~astNode() = default;
    astNodeType type = astNodeType::GENERIC;
    virtual void print(int indent = 0) const = 0;
    virtual void generateASM() const = 0;
};

struct numberNode : astNode {
    int value;

    explicit numberNode(int v) : value(v) {
        type = astNodeType::NUMBER; // Explicit because I could not figure out the other way
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "int(" << value << ")\n";
    }

    void generateASM() const override = 0; // No asm yet
};

#endif // AST_H
