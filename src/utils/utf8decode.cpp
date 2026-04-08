#include <cstdint>
#include <utility>

#include "utils/utf8.h"
using namespace std;

constexpr pair<char32_t, int> REPLACEMENT_CHARACTER = {0xFFFD, 1};

// Возвращает {кодовая_точка, длина_в_байтах} или {U+FFFD, 1} при ошибке
pair<char32_t, int> decodeUtf8At(const char *ptr, const size_t remaining) {
    const auto c = static_cast<unsigned char>(ptr[0]);

    if (c < 0x80) return {static_cast<char32_t>(c), 1};

    int len = (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : 4;
    if (static_cast<size_t>(len) > remaining)
        return REPLACEMENT_CHARACTER;

    static constexpr char32_t masks[] = {0x7F, 0x1FFF, 0xFFFF, 0x1FFFFF};
    static constexpr char32_t min_cp[] = {0x80, 0x800, 0x10000, 0};

    char32_t cp = c & masks[len-1];
    for (int i = 1; i < len; ++i) {
        const auto b = static_cast<unsigned char>(ptr[i]);
        if ((b & 0xC0) != 0x80) return REPLACEMENT_CHARACTER;
        cp = (cp << 6) | (b & 0x3F);
    }

    if (cp < min_cp[len-1] || (cp >= 0xD800 && cp <= 0xDFFF) || cp > 0x10FFFF)
        return REPLACEMENT_CHARACTER;

    if ((len == 3 && cp < 0x800) || (len == 4 && cp < 0x10000))
        return REPLACEMENT_CHARACTER;

    return {cp, len};
}