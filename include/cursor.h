#pragma once
#include <cstddef>
#include "token.h"

class Lexer;

struct Cursor {
	size_t position = 0;
	union {
		Position lc;
		struct {
			size_t line;
			size_t column;
		};
	};
	Lexer* parent = nullptr;
	
	Cursor(Lexer* p) : parent(p) {
		line = 1;
		column = 1;
	};
	
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
	inline void moveWhile(bool condition) {
		while (condition) {
			if (this->read() == '\n') this->newLn();
			else this->fwd();
		};
	};
	inline void setTo(Cursor& other) {
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