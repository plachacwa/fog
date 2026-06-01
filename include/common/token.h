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
    int rawIndex    = 0;
    int line        = 1;
    int column      = 1;

    [[nodiscard]] CompactPosition compact() const {
        return CompactPosition{line, column};
    };
};

enum class TokenType {
    // Value types
    Integer,
    Float,
    PrefixedInt,
    Exponential,
    Char,
    String,
    // Symbol type
    Symbol,
    PossibleFlag,
    // Special types
    WeakKeyword,
    Keyword,
    Operator,
    // Less-semantic types
    Directive,
    // Punctuation types
    LCB, // {   Left  Curly  Bracket, 12
    RCB, // }   Right Curly  Bracket, 13
    LSB, // [   Left  Square Bracket, 14
    RSB, // ]   Right Square Bracket, 15
    LRB, // (   Left  Round  Bracket, 16
    RRB, // )   Right Round  Bracket, 17
    Colon,
    Semicolon,
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

struct Line {
    int startIndex = 0;
    int endIndex   = 0;
    bool isEmpty   = true;
};