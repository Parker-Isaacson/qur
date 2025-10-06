#ifndef LEXER_H
#define LEXER_H

#include <exception>
#include <string>
#include <fstream>
#include <vector>

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
    // Unknown for the sake that it's a possibility / edge case
    UNKNOWN,

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
    IDENTIFIER, // variable

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
    DOUBLE, // double
    BOOLEAN, // boolean
    CHAR, // char
    STRING, // string
/* Not implemented in the first interation of this compiler.
    LIST, // list
    TUPLE, // tuple
    DICT, // dict
    TYPE, // type
*/

    // Operators
    ASSIGN, // =
/* Not implemented in the first interation of this compiler.
    SETEACH, // ->
*/
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
};

class lexer {
private:
    std::string inFile_;
    std::ifstream inStream_;
    std::vector<Token> tokens_;

public:
    lexer(const std::string& inFile);
    ~lexer();
    std::vector<Token> getTokens() { return tokens_; }
    void printTokens();
};

#endif // LEXER_H
