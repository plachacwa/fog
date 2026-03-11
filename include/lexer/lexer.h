#pragma once
#include <unordered_set>
#include <vector>
#include <expected>

#include "common/region.h"
#include "common/token.h"
#include "tokenfactory.h"
#include "reader/reader.h"

class Lexer {
    Reader &reader;
    BigPosition currentPosition;
    Region errors;
    std::unordered_set<Error*> errorsList;
    TokenFactory tf;

    friend class TokenFactory;

    public:
        explicit Lexer(Reader&);
        std::vector<Token> tokenizeAll();
        Token nextToken();

    private:
        void skipWhitespace();
        Token scanSymbol();

        Token scanDigit();
        Token scanDigitPrefixed(char maxValid);
        void scanDigitStandard();
        Token scanDigitExponent();
        std::expected<bool, Error> maybeFloat(bool isFloat) const;
        std::expected<char, Error> getMaxFromPrefix(char c) const;

        Token scanChar();
        Token scanString();
        std::optional<Error> processEscSeq();
        Token scanOperator();
        Token scanPunct();

        void skipComment();
        void skipBefore(const char *endingSeq);

        Token makeToken(TokenType) const;

        Error error(std::string msg) const;
};