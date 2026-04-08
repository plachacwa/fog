#pragma once
#include "utils/utf8.h"

using Codepoint = char32_t;

class U8Char {
    Codepoint codepoint_  : 21  = 0;
    unsigned char length_ : 3   = 1;
    const char* ptr_ = nullptr;

    public:
        U8Char() = default;
        U8Char(const char* ptr, const std::size_t remaining) : ptr_(ptr) {
            auto [fst, snd] = decodeUtf8At(ptr, remaining);
            codepoint_ = fst;
            length_   = snd;
        };

        [[nodiscard]] Codepoint codepoint()  const { return codepoint_; };
        [[nodiscard]] int       length()     const { return length_; };
        [[nodiscard]] const char* ptr()     const { return ptr_; };

        [[nodiscard]] bool      isAscii()    const { return codepoint_ <= 0x7F; };
        [[nodiscard]] char      ascii()      const { return isAscii() ? static_cast<char>(codepoint_) : -1; };

        [[nodiscard]] Codepoint operator*() const  { return codepoint_; };
};