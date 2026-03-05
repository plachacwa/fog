#pragma once
#include <optional>
#include <vector>

#include "common/token.h"
#include "reader/reader.h"

class Lexer {
    Reader &reader;
    Position currentPosition;

    public:
        Lexer(Reader&);
        std::vector<Token> tokenizeAll();
        Token nextToken();

    private:
        void skipWhitespace() const;

        Token makeToken(TokenType);
};