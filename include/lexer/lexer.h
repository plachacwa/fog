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

    Token scanSymbol();

    Token scanDigit();

    void scanDigitPrefixed();

    void scanDigitStandard(bool &isFloat);

    void scanDigitExponent();

    bool maybeFloat(bool isFloat, bool isExponent);

    char getMaxFromPrefix(char c);

    Token scanChar();

    Token scanString();

    void processEscSeq();

    Token scanOperator();

    Token scanPunct();

    void skipSpace();

    void processNewLn();

    void skipComment();

    Token makeToken(TokenType);

    Token makeToken(TokenType type, std::string_view lexeme);
};