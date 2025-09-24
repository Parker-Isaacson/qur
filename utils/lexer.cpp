#include "lexer.h"

std::string TokenToString(TokenType type) {
    switch (type) {
        case TokenType::RETURN: return "return";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACK: return "[";
        case TokenType::RBRACK: return "]";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COLON: return ":";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::IDENTIFIER: return "variable";
        case TokenType::IF: return "if";
        case TokenType::ELSEIF: return "elif";
        case TokenType::ELSE: return "else";
        case TokenType::FOR: return "for";
        case TokenType::WHILE: return "while";
        case TokenType::CONTINUE: return "continue";
        case TokenType::BREAK: return "break";
        case TokenType::FUNCTION: return "fn";
        case TokenType::LITERAL: return "literal";
        case TokenType::VOID: return "void";
        case TokenType::INT: return "int";
        case TokenType::DOUBLE: return "double";
        case TokenType::BOOLEAN: return "boolean";
        case TokenType::CHAR: return "char";
        case TokenType::STRING: return "string";
        case TokenType::ASSIGN: return "=";
        case TokenType::ADD: return "+";
        case TokenType::SUB: return "-";
        case TokenType::MUL: return "*";
        case TokenType::DIV: return "/";
        case TokenType::MOD: return "%";
        case TokenType::LESSTHAN: return "<";
        case TokenType::MORETHAN: return ">";
        case TokenType::LESSTHANEQUAL: return "<=";
        case TokenType::MORETHANEQUAL: return ">=";
        case TokenType::EQUAL: return "==";
        case TokenType::NOTEQUAL: return "!=";
        case TokenType::NOT: return "!";
        case TokenType::AND: return "&";
        case TokenType::OR: return "|";
        case TokenType::INVERT: return "~";
        default: return "UNKNOWN";
    }
}

TokenType StringToToken(const std::string& str) {
    if (str == "return") return TokenType::RETURN;
    if (str == "{") return TokenType::LBRACE;
    if (str == "}") return TokenType::RBRACE;
    if (str == "(") return TokenType::LPAREN;
    if (str == ")") return TokenType::RPAREN;
    if (str == "[") return TokenType::LBRACK;
    if (str == "]") return TokenType::RBRACK;
    if (str == ";") return TokenType::SEMICOLON;
    if (str == ":") return TokenType::COLON;
    if (str == ",") return TokenType::COMMA;
    if (str == ".") return TokenType::DOT;
    if (str == "variable") return TokenType::IDENTIFIER;
    if (str == "if") return TokenType::IF;
    if (str == "elif") return TokenType::ELSEIF;
    if (str == "else") return TokenType::ELSE;
    if (str == "for") return TokenType::FOR;
    if (str == "while") return TokenType::WHILE;
    if (str == "continue") return TokenType::CONTINUE;
    if (str == "break") return TokenType::BREAK;
    if (str == "fn") return TokenType::FUNCTION;
    if (str == "literal") return TokenType::LITERAL;
    if (str == "void") return TokenType::VOID;
    if (str == "int") return TokenType::INT;
    if (str == "double") return TokenType::DOUBLE;
    if (str == "boolean") return TokenType::BOOLEAN;
    if (str == "char") return TokenType::CHAR;
    if (str == "string") return TokenType::STRING;
    if (str == "=") return TokenType::ASSIGN;
    if (str == "+") return TokenType::ADD;
    if (str == "-") return TokenType::SUB;
    if (str == "*") return TokenType::MUL;
    if (str == "/") return TokenType::DIV;
    if (str == "%") return TokenType::MOD;
    if (str == "<") return TokenType::LESSTHAN;
    if (str == ">") return TokenType::MORETHAN;
    if (str == "<=") return TokenType::LESSTHANEQUAL;
    if (str == ">=") return TokenType::MORETHANEQUAL;
    if (str == "==") return TokenType::EQUAL;
    if (str == "!=") return TokenType::NOTEQUAL;
    if (str == "!") return TokenType::NOT;
    if (str == "&") return TokenType::AND;
    if (str == "|") return TokenType::OR;
    if (str == "~") return TokenType::INVERT;
    return TokenType::UNKNOWN;
}

lexer::lexer(const std::string& inFile)
    : inFile_(inFile) {
    std::cout << "Here" << std::endl;
    // Open the file
    inStream_.open(inFile);
    // We then check if the file is actually open
    if ( !inStream_.good() ) {
        throw lexerError("Error opening file");
    }

    std::string line;
    int row = 0;
    while ( std::getline(inStream_, line) ) {
        std::string unLexed = "";
        Token t;
        for ( int i = 0; i < line.size(); i++ ) {
            char c = line[i];
            switch ( c ) {
                case 'r': // Checking for return
                    if ( unLexed != "" ) {
                        unLexed += c;
                        break;
                    }
                    if ( line.compare(i, 6, "return") ) {
                        i += 5;
                        t = { TokenType::RETURN, TokenToString(TokenType::RETURN), row, i };
                        tokens_.push_back(t);
                    } else {
                        unLexed += c;
                    }
                    break;
                // These all can be ignored
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    break;
                // The following are guarantied to be a single char
                case '{':
                case '}':
                case '(':
                case ')':
                case '[':
                case ']':
                case ';':
                case ',':
                case '.':
                case '+':
                case '*':
                case '%':
                case '&':
                case '|':
                case '~':
                    // Need to handle the single char, should be as easy as making a new Token, via a utility function
                    t = { StringToToken(std::string(1, c)), std::string(1, c), row, i };
                    tokens_.push_back(t);
                    break;
                // Unknown
                default:
                    unLexed += c;
                    break;
                
            }
        }
        std::cout << std::endl;
        row++;
    }
}

lexer::~lexer() {
    inStream_.close();
}
