#pragma once
#include <cstddef>
#include "token.h"
#include "globalctx.h"
#include "scope.h"

class Parser;

struct PCursor {
	size_t position;
	Parser *parent;
	Scope *scope;
	GlobalContext *ctx;
	
	PCursor(Parser* p, GlobalContext* c) : position(0), parent(p), ctx(c) {};
	
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