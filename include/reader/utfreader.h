#pragma once
#include <vector>

#include "common/token.h"
#include "reader/ireader.h"
#include "lexer/u8char.h"

class UTFReader : public IReader {
    BigPosition position_;
    const std::string_view source;
    std::vector<U8Char> bufferView;

    public:
        explicit UTFReader(const std::string_view s) : IReader(), source(s) {};
        ~UTFReader() override = default;

        bool move(int) override;
        [[nodiscard]] std::string_view substrFrom(BigPosition) const override;

        [[nodiscard]] Codepoint readChar() const override     { return source[position_.rawIndex]; };
        [[nodiscard]] Codepoint readNextChar() const override { return readCharAt(1); };
        [[nodiscard]] Codepoint readCharAt(const int offset) const override {
            return readUtf8CharAt(offset).ascii();
        };
        [[nodiscard]] U8Char readUtf8CharAt(const int offset) const override {
            if (isOutsideOfBuffer(offset)) {
                const int bufIndex = (position_.column-1) % LIMIT_BUFFER_SIZE;
                return bufferView[bufIndex + offset];
            } else
                return getCharPtrOutsideBufferAt(offset);
        };
        BigPosition& position() override { return position_; };
        [[nodiscard]] std::size_t sourceSize() const override { return source.size(); };

    private:
        static constexpr int EXPANDED_BUFFER_SIZE = 256;
        static constexpr int EXPANSION_SIZE = 32;
        static constexpr int LIMIT_BUFFER_SIZE = EXPANDED_BUFFER_SIZE - EXPANSION_SIZE;

        void bufferNext();
        void expandBuffer();
        void fillBufferFromTo(int, int);
        void catchLongStrings();

        [[nodiscard]] U8Char getCharPtrOutsideBufferAt(int) const;
        [[nodiscard]] bool isOutsideOfBuffer(const int offset) const {
            const int bufIndex = (position_.column-1) % LIMIT_BUFFER_SIZE;
            return bufIndex + offset < static_cast<int>(bufferView.size()); // the size of the buffer is always integer
        };
};
