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
		"get",		"if",		"import",	"interface",
		"let",		"match",	"module",	"move",
		"null",		"private",	"protected","public",
		"return",	"spawn",	"set",		"this",	
		"true",		"type",		"var",		"where",
		"while",	"yield"
    };
    return table;
};

static constexpr
string_view opSymbols = "!#$%&*+,-./<=>?@^`|~\\";


Lexer::Lexer(string_view code) : source(code), backCur(this), frontCur(this), end(source.size()) {};

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
		backCur.setTo(frontCur);
		const char c = frontCur.read();
	
		if (c == '\0') return makeToken(Token::End);
		if (c == '\n') { processNewLn(); continue; }
		if (isspace(c)) 			{ skipSpace(); continue; };
		if (is(c).symStart()) 		return scanSymbol();
		if (isdigit(c)) 			return scanDigit();
		if (c == '\'')  			return scanChar();
		if (c == '"')				return scanString();
		
		if (is(c).from("-{") && frontCur.readNext() == '-') {
			skipComment(); continue;
		};
		
		if (is(c).from(opSymbols))	return scanOperator();
		if (is(c).from("(){}[];:"))	return scanPunct();
		
		frontCur.fwd();
		return makeToken(Token::Unknown);
	};
	return makeToken(Token::End);
};


Token Lexer::scanSymbol() {
	char c;
	do { c = frontCur.fwd_read(); } while (is(c).symCont());
	
	string_view lexeme = getStringBetweenCurs();
	
	auto it = keywords().find(lexeme);
	
	if (it != keywords().end())
		return makeToken(Token::Keyword, lexeme);
	else
		return makeToken(Token::Symbol, lexeme);
};

Token Lexer::scanDigit() {
	char c = frontCur.read();
	bool isFloat = false;
	
	while (isdigit(c)) {
		if (frontCur.readNext() == '.')
			isFloat = maybeFloat(isFloat);
		c = frontCur.fwd_read();
	};
	return makeToken(isFloat ? Token::Float : Token::Integer);
};
// Cursor is located before the dot.
// Function checks whether the char after the dot (cursor + 2 chars) is a digit,
// and whether the dot is the first in the number.
bool Lexer::maybeFloat(bool isFloat) {
	const char next = frontCur.readWithOffset(2);
	
	if (isEnd(2))
		return false;
	if (isFloat)
		handleException("multiple decimal dots in the number");
	
	if (isdigit(next)) {
		frontCur.fwd(); // to dot
		frontCur.fwd(); // skip dot
		return true;
	} else
		return false;
};


Token Lexer::scanChar() {
	frontCur.fwd(); // skip '
	if (frontCur.read() == '\\')
		processEscSeq();
	else
		frontCur.fwd();
	
	if (frontCur.read() != '\'')
		handleException("unclosed char literal");
	frontCur.fwd(); // skip closing '
	return makeToken(Token::Char);
};

Token Lexer::scanString() {
	frontCur.fwd(); // skip opening "
	char c = frontCur.read();
	
	do {
		if (c == '\0')
			handleException("unclosed string literal");
		if (c == '\\')
			processEscSeq();
		c = frontCur.fwd_read();
	} while (c != '"');
	
	frontCur.fwd(); // skip closing "
	return makeToken(Token::String);
};

// The function considers that cursor is on the \
// and does not check if this is really the case.
void Lexer::processEscSeq() {
	frontCur.fwd();
	const char c = frontCur.read();
	int length = 0;
	if (is(c).from("\\'\"nrtbf0e")) length = 1;
	else if (c == 'x')				length = 3;
	else if (c == 'u')				length = 5;
	else if (c == 'U')				length = 9;
	else
		handleException("invalid escape sequence");
	
	for (; length > 0; length--) frontCur.fwd();
	
	return;
};

Token Lexer::scanOperator() {
	char c;
	do { c = frontCur.fwd_read(); } while (is(c).from(opSymbols));
	return makeToken(Token::Operator);
};

Token Lexer::scanPunct() {
	frontCur.fwd();
	return makeToken(Token::Punct);
};


// \n doesn't count as whitespace
void Lexer::skipSpace() {
	char c;
	do { c = frontCur.fwd_read(); } while (c != '\n' && isspace(c));
};
void Lexer::processNewLn() {
	wasNewLn = true;
	frontCur.newLn();
};
/* Comments have that syntax:
 *     -- one-line comment
 *	  {- multiline
 *		 		   comment -}
 */
void Lexer::skipComment() {
	const char c = frontCur.read();
	if (c == '{')
		skipBefore("-}");
	else
		skipBefore("\n");
};

void Lexer::skipBefore(const char* endingSeq) {
	const size_t endingPos = source.find(endingSeq, frontCur.position);
	const size_t length = strlen(endingSeq);
	
	// If ending sequence is not found, consume rest of input
	if (endingPos == string::npos || endingPos + length > end) {
		while (frontCur.position < end) frontCur.move();
		return;
	};
	
	const size_t targetPos = endingPos + length;
	while (frontCur.position != targetPos) frontCur.move();
};




bool Lexer::isEnd(int offset) const {
	return (frontCur.position + offset) >= end;
};
Token Lexer::makeToken(Token::Type tokenType) {
	return makeToken(tokenType, getStringBetweenCurs());
};
Token Lexer::makeToken(Token::Type tokenType, string_view lexeme) {
	const bool newLnFlag = wasNewLn;
	wasNewLn = false;
	
	// The back cursor slides to the front one
	// to display the correct position of the end token
	if (tokenType == Token::End)
		backCur.setTo(frontCur);
	
	return Token {
		tokenType,
		lexeme, {
			backCur.line,
			backCur.column,
		}, newLnFlag,
	};
};
string_view Lexer::getStringBetweenCurs() const {
	if (backCur.position >= source.size())
		return "";
	else
		return source.substr(backCur.position, frontCur.position - backCur.position);
};

void Lexer::handleException(const string& msg) {
	const string
		line = to_string(backCur.line),
		column = to_string(backCur.column);
	throw runtime_error(
			"Lexer ERROR: " + msg + " at line " + line + ", column " + column
		);
};