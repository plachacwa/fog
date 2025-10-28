#include "lcursor.h"
#include "lexer.h"
#include <iostream>
using namespace std;

char LCursor::readWithOffset(int offset) const {
	const size_t pos = this->position + offset;
	const size_t end = parent->end;
	
	if (pos < end)
		return parent->source[ pos ];
	else {
		return '\0';
	};
};