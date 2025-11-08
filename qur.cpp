#include <iostream>

#include "utils/lexer.h"
#include "utils/ast.h"

void debug() {
    std::cout << "Debug Called." << std::endl;
}

int main(int argc, char** argv) {
    std::string inFile = "";
    std::string outFile = "out";
    for ( int i = 1; i < argc; i++ ) { // Parse through arguments
        std::string param = std::string(argv[i]);
        if ( param == "-h" || param == "-?" || param == "--help" ) {
            std::cout << "See https://github.com/Parker-Isaacson/qur for help." << std::endl;
            return 0;
        } else if ( param == "-c" || param == "--compile" ) {
            inFile = argv[++i];
        } else if ( param == "-d" || param == "--download" ) {
            inFile = argv[++i];
        } else if ( param == "-o" || param == "--out" ) {
            outFile = argv[++i];
        } else {
            std::cout << "Bad argument: " << param << ". Skipping.\n";
        }
    }
    try {
        // Step 1: Lexical Analysis
        std::cout << "=== Lexical Analysis ===\n";
        lexer lex(inFile);
        std::vector<Token> tokens = lex.getTokens();
        
        std::cout << "Tokens: ";
        lex.printTokens();
        std::cout << "\n\n";

        // Step 2: Build AST
        std::cout << "=== Building AST ===\n";
        AST ast(tokens);
        ast.build();
        std::cout << "AST built successfully!\n\n";

        // Step 3: Print AST
        ast.print();
        std::cout << "\n";

        // Step 4: Code Generation (placeholder)
        // ast.generateCode();
    } catch (const lexerError& e) {
        std::cerr << "Lexer Error: " << e.what() << std::endl;
        return 1;
    } catch (const astError& e) {
        std::cerr << "AST Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
