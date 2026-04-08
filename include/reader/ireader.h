#pragma once
#include <memory>
#include <optional>
#include <string>

#include "common/token.h"
#include "lexer/u8char.h"

class IReader {
	public:
		IReader() = default;
		virtual ~IReader() = default;

		virtual bool move(int) = 0;
		[[nodiscard]] virtual Codepoint readChar() const = 0;
		[[nodiscard]] virtual Codepoint readNextChar() const = 0;
		// The function is designed for offsets < EXPANSION_SIZE, the passes behind/after it may be slower
		[[nodiscard]] virtual Codepoint readCharAt(int) const = 0;
		// The function is designed for offsets < EXPANSION_SIZE, the passes behind/after it may be slower
		[[nodiscard]] virtual U8Char readUtf8CharAt(int) const = 0;
		[[nodiscard]] virtual std::string_view substrFrom(BigPosition) const = 0;

		virtual BigPosition& position() = 0;
		[[nodiscard]] virtual std::size_t sourceSize() const = 0;

		bool move() { return move(1); };
		[[nodiscard]] Codepoint  readCharAt() const { return readCharAt(0); };
		[[nodiscard]] U8Char readUtf8Char() const { return readUtf8CharAt(0); };
};

std::optional<std::string> readFile(const std::string &path);