#pragma once
#include <cstdint>

namespace Type {
	enum class Token : uint8_t {
		Symbol,
		Integer,
		Float,
		Char,
		String,
		Operator,
		Keyword,
		Punct,
		End,
		Unknown,
	};
};