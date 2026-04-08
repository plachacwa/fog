#include <iostream>
#include <optional>

#include "reader/standardreader.h"
#include "reader/utfreader.h"
#include "lexer/lexer.h"
using namespace std;

optional<string> getPath(int argc, char* argv[]);
void printToken(const Token &t);

int main(int argc, char* argv[]) {
    const auto code = getPath(argc, argv).and_then(readFile);
    if (!code) return 1;

    auto reader = UTFReader(string_view(*code));
    auto tokens  = Lexer(reader).tokenizeAll();

    for (auto &t : tokens)
        printToken(t);

    return 0;
};
