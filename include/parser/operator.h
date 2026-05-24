#pragma once
#include <cstdint>

struct Op {
	enum Associativity : std::uint8_t { NonAssociative, LeftAssociative, RightAssociative, EqAssociative };
	enum Arity		   : std::uint8_t { Noughtary, UnaryPrefix, UnaryPostfix, Binary, Multiple };
	
	const char *lexeme;
	Associativity associativity;
	Arity arity;
};