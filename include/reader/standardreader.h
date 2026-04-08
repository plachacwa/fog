#pragma once
#include <string>

#include "common/token.h"
#include "reader/ireader.h"

class StandardReader : public IReader {
    BigPosition position_;
    std::string source_;

    private:
        StandardReader() : IReader() {} ;
        ~StandardReader() override = default;

    public:
        bool move(int) override;
        [[nodiscard]] Codepoint readChar() const override;
        [[nodiscard]] Codepoint readNextChar() const override;
        [[nodiscard]] Codepoint readCharAt(int) const override;
        [[nodiscard]] std::string_view substrFrom(BigPosition) const override;

        BigPosition& position() override { return position_; };
        [[nodiscard]] std::size_t sourceSize() const override { return source_.size(); };
};
