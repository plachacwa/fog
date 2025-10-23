#include "lexer.h"

Carriage::Carriage(Lexer* p) : parent(p) {};

void Carriage::fwd() {
	position++;
	column++;
};

void Carriage::newLn() {
	position++;
	line++;
	column = 1;
};

void Carriage::setTo(Carriage& other) {
	position = other.position;
	line = other.line;
	column = other.column;
};

char Carriage::getChar() {
	return getCharWithOffset(0);
};

char Carriage::getNextChar() {
	return getCharWithOffset(1);
};

char Carriage::getCharWithOffset(int offset = 0) const {
	const size_t pos = this->position + offset;
	const size_t end = parent->source.size();
	
	if (pos < end)
		return parent->source[ pos ];
	else
		return '\0';
};