#pragma once
#include <locale>
#include <algorithm>
#include <uni_algo/prop.h>
#include <unicode-xid.hh>

#include "lexer/u8char.h"

namespace ext = una::codepoint;
namespace xid = unicode_xid;

namespace Charset {
    using Checker = bool (*)(Codepoint);

    constexpr Checker AnyExceptNull  = [](Codepoint c) { return c != '\0'; };
    constexpr Checker Digit          = [](Codepoint c) { return (c >= '0'  && c <= '9')  || c == '\''; };
    constexpr Checker DigitAndDot    = [](Codepoint c) { return Digit(c)   || c == '.';  };
    constexpr Checker SymStart       = [](Codepoint c) { return xid::is_xid_start(c)    || c == '_';   };
    constexpr Checker SymCont        = [](Codepoint c) { return xid::is_xid_continue(c) || c == '\'';  };
    constexpr Checker Whitespace     = [](Codepoint c) { return ext::is_whitespace(c);  };
    constexpr Checker Operator       = [](Codepoint c) {
        return std::ranges::contains("!$%&*+,-./<=>?@^`|~", c);
    };
    constexpr Checker Punct          = [](Codepoint c) {
        return std::ranges::contains("()[]{};:", c);
    };;
    constexpr Checker PlusMinus      = [](Codepoint c) { return c == '+' || c == '-'; };
    constexpr Checker ExponentPrefix = [](Codepoint c) { return c == 'e' || c == 'E'; };
    constexpr Checker OneCharEscSeqs    = [](Codepoint c) {
        return std::ranges::contains(R"("'\nrt0be)", c);
    };
};
