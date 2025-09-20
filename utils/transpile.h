#ifndef TRANSPILE_H
#define TRANSPILE_H

#include <exception>
#include <string>
#include <fstream>

#include <iostream>

class tException : public std::exception {
private:
    std::string msg_;

public:
    tException(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override {
        return msg_.c_str();
    }
};

class tQur {
private:
    std::string inFile_;
    std::string outFile_;
    std::ifstream inStream_;
    std::ofstream outStream_;
public:
    tQur(const std::string& inFile, const std::string& outFile);
    ~tQur();
    int go();
};

#endif // TRANSPILE_H