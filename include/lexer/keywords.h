#pragma once
#include <optional>
#include <string_view>

constexpr char WEAK_KEYWORD_DELIMITER = 50;

enum class Keyword {
    WC, // U+005F (_)
    Await,
    Break,
    Class,
    Const,
    Continue,
    Else,
    Enum,
    Extend,
    Extern,
    False,
    Final,
    For,
    If,
    In,
    Interface,
    Internal,
    Let,
    Match,
    Module,
    Move,
    Null,
    Private,
    Protected,
    Public,
    Return,
    Spawn,
    Static,
    Super,
    This,
    True,
    Try,
    Type,
    Typeof,
    Var,
    Void,
    Where,
    While,
    Yield,
    // Weak
    Get = WEAK_KEYWORD_DELIMITER,
    Include,
    Set
};

std::optional<Keyword> getKeyword(std::string_view);