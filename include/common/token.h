#pragma once
#include <string_view>

struct Position {
    int index;
    int line;
    int column;
};

enum class TokenType {
    Integer,
    Float,
    String,
    Char,
    Operator,
    Unknown,
    End
};

struct Token {
    std::string_view lexeme;
    TokenType type;
    Position position;
};