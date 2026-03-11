#include <iomanip>
#include <optional>
#include <string>
#include <iostream>

#include "common/token.h"
using namespace std;

optional<string> getPath(int argc, char* argv[]) {
    if (argc < 2) return nullopt;
    return argv[1];
};

void printToken(const Token &t) {
    cout << t.type_str() << ": ";
    cout << t.lexeme << " ";
    cout << "[" << t.position.line
    << ":" << t.position.column << "]";
    cout << '\n';
};