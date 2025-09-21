#include "lexer.h"

lexer::lexer(const std::string& inFile)
    : inFile_(inFile) {
    inStream_.open(inFile);
}

lexer::~lexer() {
    inStream_.close();
}