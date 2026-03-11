#include <fstream>
#include <ranges>

#include "reader/reader.h"

#include <cassert>
#include <cmath>
using namespace std;

Reader::Reader()
    : position() {};

optional<Reader> Reader::readFile(const string &path) {
    ifstream file(path);
    if (!file.is_open())
        return nullopt;

    auto range = ranges::subrange(
        istreambuf_iterator<char>(file),
        istreambuf_iterator<char>()
    );

    auto reader = Reader();
    reader.source = string(range.begin(), range.end());
    return reader;
};

Reader Reader::readString(string &&str) {
    auto reader = Reader();
    reader.source = str;
    return reader;
}

bool Reader::move(int steps) {
    while (steps > 0) {
        position.index++;
        switch (readChar()) {
            case '\0': return false;
            case '\n': position.line++;
                       position.column = 1;
                       break;
            default:   position.column++;
        };
        steps--;
    };
    return true;
};

char Reader::readChar() const { return readCharWithOffset(0); };
char Reader::readNextChar() const { return readCharWithOffset(1); };
char Reader::readCharWithOffset(int offset) const {
    assert(source.length() < std::pow(2, sizeof(int)));
    if (position.index + offset >= static_cast<int>(source.length()))
        return '\0';
    else
        return source[position.index + offset];
};


string_view Reader::substrFrom(BigPosition startPosition) const {
    return string_view(source).substr(startPosition.index, position.index - startPosition.index);
};