#include <iostream>

#include "utils/lexer.h"

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
    lexer myLexer("README.md");
    myLexer.getTokens();
    std::cout << std::endl;
    return 0;
}
