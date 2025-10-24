#include <string_view>
#include <vector>
#include <string>
#include "token.h"
#include "carriage.h"

class Lexer {
public:
	Lexer(std::string_view);
	std::vector<Token> tokenize();
	
private:
	std::string_view source;
	Carriage backCar, frontCar;
	size_t end;
	bool wasNewLn = true;
	
	Token scanToken();
	Token skipSpace();
	Token scanNewLn();
	Token scanSymbol();
	
	Token scanDigit();
	bool maybeFloat(bool);
	
	Token scanChar();
	Token scanString();
	
	Token scanComment();
	void  skipBefore(const char*);
	
	Token scanOperator();
	Token scanPunct();
	
	friend char Carriage::readWithOffset(int) const;
	
	bool isEnd(int offset = 0) const;
	Token makeToken(Token::Type) const;
	Token makeToken(Token::Type, std::string_view) const;
	std::string_view getStringBetweenCars() const;
	void handleException(std::string);
};