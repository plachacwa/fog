#pragma once
#include <cstdint>
#include <string_view>

namespace TypeEnums {
	enum class TokenT : uint8_t {
		Symbol,
		Integer,
		PrefixedInt,
		Float,
		Exponential,
		Char,
		String,
		Operator,
		KeywordAmbg,
		KeywordFuncLike,
		KeywordStmt,
		KeywordValue,
		Punct,
		End,
		Unknown,
	};

	std::string_view get(TokenT val);
};
namespace OperatorEnums {
	enum class Associativity : uint8_t {
		NotAssociative,
		EqualAssociative,
		LeftAssociative,
		RightAssociative
	};
	enum class Kind : uint8_t {
		Unary,
		Binary,
		Multiple
	};
};
