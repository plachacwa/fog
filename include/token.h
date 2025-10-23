#pragma once
#include <enums.h>
#include <string_view>

struct Position {
	size_t line;
	size_t column;
};

struct Token {
	using Type = Type::Token;
	using enum ::Type::Token;
	
	Type type;
	std::string_view lexeme;
	Position pos;
	
	size_t &line = pos.line;
	size_t &column = pos.column;
};