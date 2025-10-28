#include "parser.h"
#include <iostream>
using namespace std;

Token& PCursor::readWithOffset(int offset) const {
	const size_t pos = this->position + offset;
	const size_t end = parent->end;
	
	if (pos < end)
		return parent->tokens[ pos ];
	else {
		return parent->tokens[ end-1 ];
	};
};
bool PCursor::isEnd(int offset) const {
	return (this->position + offset) >= parent->end;
}