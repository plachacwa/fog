#include "reader/utfreader.h"
#include "utils/utf8.h"
#include <uni_algo/ranges_conv.h>

using namespace std;

bool UTFReader::move(int steps) {
    while (steps > 0) {
        const auto [codepoint, length] =
            decodeUtf8At(source.data() + position_.rawIndex,
                         source.length() - position_.rawIndex);

        position_.index++;
        position_.rawIndex += length;

        switch (codepoint) {
            case '\0':
                return false;
            case '\n':
                position_.line++;
                position_.column = 1;
                bufferNext();
                break;
            default:
                position_.column++;
                catchLongStrings();
        };
        steps--;
    };
    return true;
};

void UTFReader::bufferNext() {
    fillBufferFromTo(0, LIMIT_BUFFER_SIZE);
};
void UTFReader::expandBuffer() {
    fillBufferFromTo(LIMIT_BUFFER_SIZE, EXPANDED_BUFFER_SIZE);
};
void UTFReader::fillBufferFromTo(const int start, const int limit) {
    const char *begin = source.data() + position_.rawIndex;
    const char *cursor = begin;
    const char *end = source.data() + source.length();
    bufferView.resize(limit);

    for (int i = start; i < limit; i++) {
        const U8Char c(cursor, end - cursor);
        cursor += c.length();
        bufferView.at(i) = c;

        if (*c == U'\n')
            break;
    };
};
void UTFReader::catchLongStrings() {
    const int col = position_.column-1;
    if (col / LIMIT_BUFFER_SIZE == 0)
        return;

    const int bufIndex = col % LIMIT_BUFFER_SIZE;
    if (bufIndex == EXPANSION_SIZE)
        bufferNext();
    if (bufIndex == 0)
        expandBuffer();
};

U8Char UTFReader::getCharPtrOutsideBufferAt(const int offset) const {
    const char *cursor = source.data() + position_.rawIndex;
    int steps = offset;

    while (steps > 0) {
        const size_t remaining = source.data() + source.length() - cursor;
        const auto length = decodeUtf8At(cursor, remaining).second;
        cursor += length;
        steps--;
    };

    return U8Char(cursor, source.data() + source.length() - cursor);
};

string_view UTFReader::substrFrom(const BigPosition startPosition) const {
    const int distance = position_.rawIndex - startPosition.rawIndex;
    return source.substr(startPosition.rawIndex, distance);
};