#include <iostream>

#include "utils/transpile.h"

void debug() {
    std::cout << "Debug Called." << std::endl;
}

enum Method {
    COMPILE,
    TRANSPILE,
    DOWNLOAD,
    UNKNOWN
} method;

int main(int argc, char** argv) {
    std::string inFile = "";
    std::string outFile = "out";
    method = UNKNOWN;
    for ( int i = 1; i < argc; i++ ) { // Parse through arguments
        std::string param = std::string(argv[i]);
        if ( param == "-h" || param == "-?" || param == "--help" ) {
            std::cout << "See https://github.com/Parker-Isaacson/qur for help." << std::endl;
            return 0;
        } else if ( param == "-c" || param == "--compile" ) {
            inFile = argv[++i];
            method = COMPILE;
        } else if ( param == "-p" || param == "--transpile" ) {
            inFile = argv[++i];
            method = TRANSPILE;
        } else if ( param == "-d" || param == "--download" ) {
            inFile = argv[++i];
            method = DOWNLOAD;
        } else if ( param == "-o" || param == "--out" ) {
            outFile = argv[++i];
        } else {
            std::cout << "Bad argument: " << param << ". Skipping.\n";
        }
    }

    if ( method == UNKNOWN ) {
        std::cout << "Bad calling, see https://github.com/Parker-Isaacson/qur for help." << std::endl;
        return 1;
    } if ( method == COMPILE || method == DOWNLOAD ) {
        std::cout << "Not built yet, --transpile is the only valid option" << std::endl;
        return 1;
    } else if ( method == TRANSPILE ) {
        try {
            tQur transpiler(inFile, outFile);
            transpiler.go();
        } catch (...) {
            std::cout << "An error has occurred!" << std::endl;
            return 2;
        }
    } else {
        std::cout << "Unknown state: " << method << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    return 0;
}