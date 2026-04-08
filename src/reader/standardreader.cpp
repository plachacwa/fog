#include <fstream>
#include <ranges>

#include "reader/standardreader.h"

#include <cassert>
#include <cmath>
using namespace std;

bool StandardReader::move(int steps) {
    while (steps > 0) {
        position_.index++;
        switch (readChar()) {
            case '\0': return false;
            case '\n': position_.line++;
                       position_.column = 1;
                       break;
            default:   position_.column++;
        };
        steps--;
    };
    return true;
};

Codepoint StandardReader::readChar() const { return readCharAt(0); };
Codepoint StandardReader::readNextChar() const { return readCharAt(1); };
Codepoint StandardReader::readCharAt(const int offset) const {
    if (source_.length() > static_cast<size_t>(std::numeric_limits<int>::max()))
        throw std::runtime_error("File too large: cannot jump more than 2GB");
    if (position_.index + offset >= static_cast<int>(source_.length()))
        return '\0';
    else
        return source_[position_.index + offset];
};


string_view StandardReader::substrFrom(const BigPosition startPosition) const {
    return string_view(source_).substr(startPosition.index, position_.index - startPosition.index);
};