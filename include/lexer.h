#include <string_view>
#include <vector>
#include "token.h"

class Lexer;

// carriage.cpp
struct Carriage {
	size_t position = 0;
	Position lc;
	size_t &line = lc.line;
	size_t &column = lc.column;
	Lexer* parent = nullptr;
	
	Carriage(Lexer*);
	
	void fwd();
	void newLn();
	void setTo(Carriage&);
	char getChar();
	char getNextChar();
	char getCharWithOffset(int) const;
};

// lexer.cpp
class Lexer {
public:
	Lexer(std::string_view);
	std::vector<Token> tokenize;
	
private:
	std::string_view source;
	Carriage backCar, frontCar;
	
	friend char Carriage::getCharWithOffset(int offset) const;
};