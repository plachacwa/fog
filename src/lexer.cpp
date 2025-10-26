#include "lexer.h"
#include "utils.h"
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <iostream>
using namespace std;

const unordered_set<string_view>& keywords() {
    static const unordered_set<string_view> table {
		"break",	"class",	"continue",	"const",
		"else",		"enum",		"extern",	"extend",
		"false",	"final",	"for",		"friend",
		"if",		"import",	"interface","let",
		"match",	"module",	"move",		"private",
		"protected","public",	"return",	"spawn",
		"this",		"true",		"type",		"var",
		"where",	"while",	"yield"
    };
    return table;
};

static constexpr string_view opSymbols = "!#$%&*+,-./<=>?@^`|~\\";


Lexer::Lexer(string_view code) : source(code), backCar(this), frontCar(this), end(source.size()) {};

vector<Token> Lexer::tokenize() {
	vector<Token> tokens;
	do {
		tokens.push_back(scanToken());
	}   while (!isEnd());
	if (tokens.back().type != Token::End)
		tokens.push_back(makeToken(Token::End, ""));
	return tokens;
};

Token Lexer::scanToken() {
	while (!isEnd()) {
		backCar.setTo(frontCar);
		const char c = frontCar.read();
	
		if (c == '\0') return makeToken(Token::End);
		if (c == '\n') { processNewLn(); continue; }
		if (isspace(c)) 			{ skipSpace(); continue; };
		if (is(c).symStart()) 		return scanSymbol();
		if (isdigit(c)) 			return scanDigit();
		if (c == '\'')  			return scanChar();
		if (c == '"')				return scanString();
		
		if (is(c).from("-{") && frontCar.readNext() == '-') {
			skipComment(); continue;
		};
		
		if (is(c).from(opSymbols))	return scanOperator();
		if (is(c).from("(){}[];:"))	return scanPunct();
		
		frontCar.fwd();
		return makeToken(Token::Unknown);
	};
	return makeToken(Token::End);
};


Token Lexer::scanSymbol() {
	char c;
	do { c = frontCar.fwd_read(); } while (is(c).symCont());
	
	string_view lexeme = getStringBetweenCars();
	
	auto it = keywords().find(lexeme);
	
	if (it != keywords().end())
		return makeToken(Token::Keyword, lexeme);
	else
		return makeToken(Token::Symbol, lexeme);
};

Token Lexer::scanDigit() {
	char c = frontCar.read();
	bool isFloat = false;
	
	while (isdigit(c)) {
		if (frontCar.readNext() == '.')
			isFloat = maybeFloat(isFloat);
		c = frontCar.fwd_read();
	};
	return makeToken(isFloat ? Token::Float : Token::Integer);
};
// Carriage is located before the dot.
// Function checks whether the char after the dot (carriage + 2 chars) is a digit,
// and whether the dot is the first in the number.
bool Lexer::maybeFloat(bool isFloat) {
	const char next = frontCar.readWithOffset(2);
	
	if (isEnd(2))
		return false;
	if (isFloat)
		handleException("multiple decimal dots in the number");
	
	if (isdigit(next)) {
		frontCar.fwd(); // to dot
		frontCar.fwd(); // skip dot
		return true;
	} else
		return false;
};


Token Lexer::scanChar() {
	frontCar.fwd(); // skip '
	if (frontCar.read() == '\\')
		processEscSeq();
	else
		frontCar.fwd();
	
	if (frontCar.read() != '\'')
		handleException("unclosed char literal");
	frontCar.fwd(); // skip closing '
	return makeToken(Token::Char);
};

Token Lexer::scanString() {
	frontCar.fwd(); // skip opening "
	char c = frontCar.read();
	
	do {
		if (c == '\0')
			handleException("unclosed string literal");
		if (c == '\\')
			processEscSeq();
		c = frontCar.fwd_read();
	} while (c != '"');
	
	frontCar.fwd(); // skip closing "
	return makeToken(Token::String);
};

// The function considers that carriage is on the \
// and does not check if this is really the case.
void Lexer::processEscSeq() {
	frontCar.fwd();
	const char c = frontCar.read();
	int length = 0;
	if (is(c).from("\\'\"nrtbf0e")) length = 1;
	else if (c == 'x')				length = 3;
	else if (c == 'u')				length = 5;
	else if (c == 'U')				length = 9;
	else
		handleException("invalid escape sequence");
	
	for (; length > 0; length--) frontCar.fwd();
	
	return;
};

Token Lexer::scanOperator() {
	char c;
	do { c = frontCar.fwd_read(); } while (is(c).from(opSymbols));
	return makeToken(Token::Operator);
};

Token Lexer::scanPunct() {
	frontCar.fwd();
	return makeToken(Token::Punct);
};


// \n doesn't count as whitespace
void Lexer::skipSpace() {
	char c;
	do { c = frontCar.fwd_read(); } while (c != '\n' && isspace(c));
};
void Lexer::processNewLn() {
	wasNewLn = true;
	frontCar.newLn();
};
/* Comments have that syntax:
 *     -- one-line comment
 *	  {- multiline
 *		 		   comment -}
 */
void Lexer::skipComment() {
	const char c = frontCar.read();
	if (c == '{')
		skipBefore("-}");
	else
		skipBefore("\n");
};

void Lexer::skipBefore(const char* endingSeq) {
	const size_t endingPos = source.find(endingSeq, frontCar.position);
	const size_t length = strlen(endingSeq);
	
	// If ending sequence is not found, consume rest of input
	if (endingPos == string::npos || endingPos + length > end) {
		frontCar.moveWhile(frontCar.position < end);
		return;
	};
	
	const size_t targetPos = endingPos + length;
	frontCar.moveWhile(frontCar.position != targetPos);
};




bool Lexer::isEnd(int offset) const {
	return (frontCar.position + offset) >= end;
};
Token Lexer::makeToken(Token::Type tokenType) {
	return makeToken(tokenType, getStringBetweenCars());
};
Token Lexer::makeToken(Token::Type tokenType, string_view lexeme) {
	const bool newLnFlag = wasNewLn;
	wasNewLn = false;
	
	// The back carriage slides to the front one
	// to display the correct position of the end token
	if (tokenType == Token::End)
		backCar.setTo(frontCar);
	
	return Token {
		tokenType,
		lexeme, {
			backCar.line,
			backCar.column,
		}, newLnFlag,
	};
};
string_view Lexer::getStringBetweenCars() const {
	if (backCar.position >= source.size())
		return "";
	else
		return source.substr(backCar.position, frontCar.position - backCar.position);
};

void Lexer::handleException(const string& msg) {
	const string
		line = to_string(backCar.line),
		column = to_string(backCar.column);
	throw runtime_error(
			"Lexer ERROR: " + msg + " at line " + line + ", column " + column
		);
};