#include <string_view>
#include <vector>
#include <string>
#include "token.h"
#include "lcursor.h"

class Lexer {
public:
	Lexer(std::string_view);
	
		Lexer(const Lexer&) = delete;
		Lexer& operator=(const Lexer&) = delete;

		Lexer(Lexer&&) = delete;
		Lexer& operator=(Lexer&&) = delete;
	
	std::vector<Token> tokenize();
	
private:
	std::string_view source;
	LCursor backCur, frontCur;
	size_t end;
	bool wasNewLn = true;
	
	Token scanToken();
	Token scanSymbol();
	Token scanDigit();
	bool  maybeFloat(bool);
	Token scanChar();
	Token scanString();
	void  processEscSeq();
	Token scanOperator();
	Token scanPunct();
	
	void skipSpace();
	void processNewLn();
	void skipComment();
	void skipBefore(const char*);
	
	
	friend char LCursor::readWithOffset(int) const;
	
	bool isEnd(int offset = 0) const;
	Token makeToken(Token::Type);
	Token makeToken(Token::Type, std::string_view);
	std::string_view getStringBetweenCurs() const;
	void handleException(const std::string&);
};