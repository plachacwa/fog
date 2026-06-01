#pragma once
#include "common/token.h"
#include "utils/interval.h"

namespace Typeset {
    using T       = Token;
    using TT      = TokenType;
    using Checker = bool (*)(const Token &);

    constexpr Checker isValue         = [](const T &t) { return t.type <= Interval(TT::Integer, TT::String); };
    constexpr Checker isSymbol        = [](const T &t) { return t.type <= Interval(TT::Symbol, TT::WeakKeyword); };
    constexpr Checker isValueOrSymbol = [](const T &t) { return t.type <= Interval(TT::Integer, TT::WeakKeyword); };
    constexpr Checker isBracket       = [](const T &t) { return t.type <= Interval(TT::LCB, TT::RRB); };
    constexpr Checker isLeftBracket   = [](const T &t) { return isBracket(t) && (static_cast<int>(t.type) % 2 == 0); };
    constexpr Checker isRightBracket  = [](const T &t) { return isBracket(t) && (static_cast<int>(t.type) % 2 == 1); };
};
