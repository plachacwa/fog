#pragma once
#include <optional>
#include <string>

#include "common/token.h"

struct Reader {
    Position position;
    std::string source;

    private:
        Reader();

    public:
        static std::optional<Reader> readFile(const std::string&);
        static Reader readString(std::string&&);

        bool move(int = 1);
        char readChar() const;
        char readNextChar() const;
        char readCharWithOffset(int = 0) const;
        std::string_view substrFrom(Position) const;
};
