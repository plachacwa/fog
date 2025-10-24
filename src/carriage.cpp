#include "carriage.h"
#include "lexer.h"
#include <cassert>
#include <iostream>
using namespace std;

char Carriage::readWithOffset(int offset = 0) const {
	const size_t pos = this->position + offset;
	const size_t end = parent->end;
	
	if (pos < end)
		return parent->source[ pos ];
	else {
		//cerr << "READ BEYOND EOF!\n";
		//cerr << "  position = " << position << "\n";
		//cerr << "  offset   = " << offset << "\n";
		//cerr << "  pos      = " << pos << "\n";
		//err << "  end      = " << end << "\n";
		//assert(false && "readWithOffset: pos >= end");
		return '\0';
	};
};