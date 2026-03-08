#pragma once
#include <locale>
#include <algorithm>

namespace Charset {
    using Checker = bool (*)(char);

    constexpr Checker Digit         = [](char c) { return isdigit(c) || c == '\''; };
    constexpr Checker SymStart      = [](char c) { return isalpha(c) || c == '_'; };
    constexpr Checker SymCont       = [](char c) { return SymStart(c) || Digit(c); };
    constexpr Checker SymStartExt   = [](char c) { return SymStart(c) || c < 0; };
    constexpr Checker SymContExt    = [](char c) { return SymStartExt(c) || Digit(c); };
    constexpr Checker Whitespace    = [](char c) { return !!isspace(c); };
    constexpr Checker Operator      = [](char c) {
        return std::ranges::contains("!#$%&*+,-./<=>?@^`|~", c);
    };
};
