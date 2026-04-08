#include <iomanip>
#include <optional>
#include <string>
#include <iostream>

#include "common/error.h"
#include "common/token.h"
using namespace std;

optional<string> getPath(int argc, char* argv[]) {
    if (argc < 2) return nullopt;
    return argv[1];
};

void printToken(const Token &t) {
    cout << right << setw(15) << t.type_str() << ": ";
    cout << left  << setw(30) << t.lexeme << " ";
    cout << "[" << t.position.line << ":" << t.position.column << "]";
    cout << '\n';
    if (t.error)
        cout << " !!! " << t.error->msg << '\n';
};