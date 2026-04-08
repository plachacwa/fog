#pragma once
#include <string_view>
#include <optional>

struct Error;

struct CompactPosition {
    int line   = 1;
    int column = 1;
};
struct BigPosition {
    int index       = 0;
    int rawIndex   = 0;
    int line        = 1;
    int column      = 1;

    [[nodiscard]] CompactPosition compact() const {
        return CompactPosition{line, column};
    };
};

enum class TokenType {
    Integer,
    Float,
    PrefixedInt,
    Exponential,
    Keyword,
    WeakKeyword,
    Symbol,
    PossibleFlag,
    Directive,
    Char,
    String,
    Operator,
    Punct,
    Unknown,
    End,
};

struct Token {
    std::string_view lexeme;
    TokenType type;
    CompactPosition position;
    Error *error = nullptr;

    [[nodiscard]] std::string_view type_str() const;
};