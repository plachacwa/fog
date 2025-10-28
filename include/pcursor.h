#pragma once
#include <cstddef>
#include "token.h"

class Parser;

struct PCursor {
	size_t position;
	Parser* parent;
	
	PCursor(Parser* p) : position(0), parent(p) {};
	
	inline void fwd() {
		position++;
	};
	inline Token& read() const {
		return readWithOffset(0);
	};
	inline Token& readNext() const {
		return readWithOffset(1);
	};
	Token& readWithOffset(int = 0) const;
	bool isEnd(int = 0) const;
};