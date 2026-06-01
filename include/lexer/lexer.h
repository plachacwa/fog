#pragma once
#include <expected>
#include <plf_hive.h>
#include <vector>

#include "tokenfactory.h"
#include "common/token.h"

struct IReader;

class Lexer {
    IReader &         reader;
    plf::hive<Error>& errors;
    BigPosition       currentPosition;
    TokenFactory      tf;

    std::vector<Token> tokens{};

    friend class TokenFactory;

    public:
        explicit Lexer(IReader &r, plf::hive<Error> &e) noexcept : reader(r), errors(e), tf(this) {};

        std::vector<Token> tokenizeAll();

        Token nextToken();

    private:
        bool skipWhitespaceIfExist();

        Token scanSymbolOrFlag(bool isFlag);

        Token scanDigit();

        Token scanDigitPrefixed(Codepoint maxValid);

        void scanDigitStandard();

        Token scanDigitExponent();

        [[nodiscard]] std::expected<bool, std::string> maybeFloat(bool isFloat) const;

        [[nodiscard]] static std::optional<Codepoint> getMaxDigitFromPrefix(Codepoint c);

        Token scanChar();

        Token scanString();

        std::optional<std::string> processEscSeq();

        Token scanOperator();

        Token scanPunct();

        Token scanDirective();

        void skipComment();
};
