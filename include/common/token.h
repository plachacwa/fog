#pragma once
#include <string_view>

#include "error.h"

struct Position {
    int index;
    int line;
    int column;
};

enum class TokenType {
    Integer,
    Float,
    PrefixedInt,
    Exponential,
    Symbol,
    Char,
    String,
    Operator,
    Unknown,
    End
};

struct Token {
    std::string_view lexeme;
    TokenType type;
    Position position;
    Error *error;
};