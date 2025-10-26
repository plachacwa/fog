#include "lcursor.h"
#include "lexer.h"
#include <cassert>
#include <iostream>
using namespace std;

char LCursor::readWithOffset(int offset = 0) const {
	const size_t pos = this->position + offset;
	const size_t end = parent->end;
	
	if (pos < end)
		return parent->source[ pos ];
	else {
		return '\0';
	};
};