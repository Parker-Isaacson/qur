#ifndef LEXER_H
#define LEXER_H

#include <exception>
#include <string>
#include <fstream>


class lexerError : public std::exception {
private:
    std::string msg_; // No default error

public:
    lexerError(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override {
        return msg_.c_str();
    }
};

enum class TokenType {
    // Specific Keywords
    RETURN, // return
    LBRACE, // {
    RBRACE, // }
    LPAREN, // (
    RPAREN, // )
    LBRACK, // [
    RBRACK, // ]
    SEMICOLON, // ;
    COLON, // :
    COMMA, // ,
    DOT, // .

    // Methods
    IF, // if
    ELSEIF, // elif
    ELSE, // else
    FOR, // for
    WHILE, // while
    CONTINUE, // continue
    BREAK, // break
    FUNCTION, // fn
/* Not implemented in the first interation of this compiler.
    OPERATOR, // op
    IMPORT, // import
*/

    // Specific Types
    LITERAL, // has no way of stating, inferred
    VOID, // has no way of stating, inferred
    INT, // int
    DOUBLE, // double, float
    BOOLEAN, // boolean
    CHAR, // char
    STRING, // string
    LIST, // list
    TUPLE, // tuple
    DICT, // dict
    TYPE, // type

    // Operators
    ASSIGN, // =
    SETEACH, // ->
    ADD, // +
    SUB, // -
    MUL, // *
    DIV, // /
    MOD, // %
    LESSTHAN, // <
    MORETHAN, // >
    LESSTHANEQUAL, // <=
    MORETHANEQUAL, // >=
    EQUAL, // ==
    NOTEQUAL, // !=
    NOT, // !
    AND, // &
    OR, // |
    INVERT, // ~
};

struct Token {
    TokenType type;
    std::string lexme;
    int line;
    int column;
}

class lexer {
private:
    std::string inFile_;
    std::ifstream inStream_;
public:
    bool isLexable = true;
    lexer(const std::string& inFile);
    ~lexer();

};

#endif // LEXER_H