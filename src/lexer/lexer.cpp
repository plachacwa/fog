#include "lexer/lexer.h"

#include "lexer/charsets.h"
#include "reader/readstream.h"
using namespace std;

Lexer::Lexer(Reader &reader)
    : reader(reader), currentPosition() {};

vector<Token> Lexer::tokenizeAll() {
    vector<Token> tokens;
    Token token = nextToken();

    while ( token.type != TokenType::End ) {
        tokens.push_back(token);
        token = nextToken();
    };

    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    currentPosition = reader.position;
    const char c    = reader.readChar();

    if (c == '\0')              return makeToken(TokenType::End);
    if (isdigit(c))             return scanDigit();
    if (Charset::SymStart(c))   return scanSymbol();
    if (c == '\'')              return scanChar();
    if (c == '"')               return scanString();
    if (Charset::Operator(c))   return scanOperator();

    return makeToken(TokenType::Unknown);
};

void Lexer::skipWhitespace() const {
    ReadStream(&reader)
        .maybeManyFrom(Charset::Whitespace);
};