#pragma once
#include <cstddef>
#include "token.h"

class Lexer;

struct Carriage {
	size_t position = 0;
	Position lc;
	size_t &line = lc.line;
	size_t &column = lc.column;
	Lexer* parent = nullptr;
	
	Carriage(Lexer* p) : parent(p) {};
	
	inline void fwd() {
		position++;
		column++;
	};
	inline char fwd_read() {
		fwd();
		return read();
	};
	inline void newLn() {
		position++;
		line++;
		column = 1;
	};
	inline char newLn_read() {
		newLn();
		return read();
	};
	inline void setTo(Carriage& other) {
		position = other.position;
		line = other.line;
		column = other.column;
	};
	inline char read() const {
		return readWithOffset(0);
	};
	inline char readNext() const {
		return readWithOffset(1);
	};
	char readWithOffset(int) const;
};