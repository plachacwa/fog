#pragma once
#include "enums.h"
#include <string_view>

struct Position {
	size_t line;
	size_t column;
};

struct Token {
	using Type = TypeEnums::TokenT;
	using enum ::TypeEnums::TokenT;

	Type type;
	std::string_view lexeme;
	Position pos;
	bool isFirstInLine = false;

	bool is(Token& other) const {
		return
			(this->type == other.type) &&
			(this->lexeme == other.lexeme);
	};
};
