#pragma once
#include <unordered_set>
#include <vector>
#include <expected>

#include "common/region.h"
#include "common/token.h"
#include "tokenfactory.h"

struct IReader;

class Lexer {
    IReader &reader;
    BigPosition currentPosition;
    Region errors;
    std::unordered_set<Error*> errorsList;
    TokenFactory tf;

    friend class TokenFactory;

    public:
        explicit Lexer(IReader &r) noexcept : reader(r), tf(this) {};
        std::vector<Token> tokenizeAll();
        Token nextToken();

    private:
        bool skipWhitespaceIfExist();
        Token scanSymbolOrFlag(bool isFlag);

        Token scanDigit();
        Token scanDigitPrefixed(Codepoint maxValid);
        void scanDigitStandard();
        Token scanDigitExponent();
        std::expected<bool, Error> maybeFloat(bool isFloat) const;
        std::expected<Codepoint, Error> getMaxFromPrefix(Codepoint c) const;

        Token scanChar();
        Token scanString();
        std::optional<Error> processEscSeq();
        Token scanOperator();
        Token scanPunct();

        void skipComment();

        Token makeToken(TokenType) const;

        Error error(std::string msg) const;
};