#pragma once
#include <cstddef>
#include "common/token.h"

class Lexer;

struct LexerCursor {
	size_t position = 0;
	union {
		Position lc;
		struct {
			size_t line;
			size_t column;
		};
	};
	Lexer* parent = nullptr;

	LexerCursor(Lexer* p) : parent(p) {
		line = 1;
		column = 1;
	};

	inline void fwd(size_t pace = 0) {
		position += pace;
		column += pace;
	};
	inline char fwd_read(size_t pace = 0) {
		fwd(pace);
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
	inline void move() {
		if (this->read() == '\n') this->newLn();
		else this->fwd();
	};
	inline void setTo(LexerCursor& other) {
		position = other.position;
		line = other.line;
		column = other.column;
	};
	inline char read() const {
		return readWithOffset(0);
	};
	inline char read_fwd() {
		char r = read();
		fwd();
		return r;
	};
	inline char readNext() const {
		return readWithOffset(1);
	};
	char readWithOffset(int = 0) const;
};
