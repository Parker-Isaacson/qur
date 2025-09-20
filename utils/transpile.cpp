#include "transpile.h"

tQur::tQur(const std::string& inFile, const std::string& outFile)
    : inFile_(inFile), outFile_(outFile) {
    inStream_.open(inFile_);
    outStream_.open(outFile_);
}

tQur::~tQur() {
    inStream_.close();
    outStream_.close();
}

int tQur::go() {
    return 0;
}