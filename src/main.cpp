#include <optional>

#include "lexer/lexer.h"
#include "reader/standardreader.h"
#include "reader/utfreader.h"
#include "parser/parser.h"
#include "parser/optable.h"
using namespace std;

optional<string> getPath(int argc, char *argv[]);

void printToken(const Token &t);

int main(const int argc, char *argv[]) {
    const auto code = getPath(argc, argv).and_then(readFile);
    if (!code) return 1;

    auto       reader          = UTFReader(string_view(*code));
    auto       errors          = plf::hive<Error>{};
    auto       tokens = Lexer(reader, errors).tokenizeAll();
    Region     ctx{};
    const auto node = Parser(tokens, ctx).parseExpression();

    for (auto &t: tokens)
        printToken(t);
    node->print();

    return 0;
};
