                              #pragma once
#include <cstdint>
#include <string_view>

namespace Type {
	enum class TokenT : uint8_t {
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
	
	std::string_view get(TokenT val);
};