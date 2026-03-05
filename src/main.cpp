#include <iostream>
#include <optional>

#include "reader/reader.h"
#include "lexer/lexer.h"
using namespace std;

optional<string> getPath(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    auto path = getPath(argc, argv);
    if (!path) return 1;
    auto reader = Reader::readFile(*path);
    if (!reader) return 1;
    auto lexer  = Lexer(*reader).tokenizeAll();

    return 0;
};
