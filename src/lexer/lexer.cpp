#include "lexer/lexer.h"
#include "utils.h"
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <iostream>
using namespace std;

Token::Type keywordType(string_view kw) {
    static const unordered_map<string_view, char> table {
		{"break",	'f'}, {"class",		's'}, {"continue",	'f'}, {"const",		'a'},
		{"else",	'a'}, {"enum",		's'}, {"extern",	'a'}, {"extend",	's'},
		{"false",	'v'}, {"final",		'a'}, {"for",		's'}, {"friend",	'a'},
		{"get",		'a'}, {"if",		's'}, {"import",	's'}, {"interface", 's'},
		{"let",		's'}, {"match", 	's'}, {"module",	's'}, {"move",		'f'},
		{"null",	'v'}, {"private",	'a'}, {"protected",	'a'}, {"public",	'a'},
		{"return",	'f'}, {"spawn",		's'}, {"set",		'a'}, {"this",		'v'},
		{"true",	'v'}, {"type",		's'}, {"var",		's'}, {"where",		's'},
		{"while",	's'}, {"yield",		'f'}
    };

	auto it = table.find(kw);
	if (it != table.end()) {
		switch (it->second) {
			case 'a': return Token::KeywordAmbg;
			case 'f': return Token::KeywordFuncLike;
			case 's': return Token::KeywordStmt;
			case 'v': return Token::KeywordValue;
		};
	};
    return Token::Unknown;
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

	auto keywordStatus = keywordType(lexeme);

	if (keywordStatus != Token::Unknown)
		return makeToken(keywordStatus, lexeme);
	else
		return makeToken(Token::Symbol, lexeme);
};

// ===============
// NUMBER SCANNING
// ===============

Token Lexer::scanDigit() {
	if (frontCur.read() == '0' && getMaxFromPrefix(frontCur.readNext())) {
		scanDigitPrefixed();
		return makeToken(Token::PrefixedInt);
	} else {
		bool isFloat = false;
		scanDigitStandart(isFloat);
		if (frontCur.read() == 'e' || frontCur.read() == 'E') {
			scanDigitExponent();
			return makeToken(Token::Exponential);
		};
		return makeToken(isFloat ? Token::Float : Token::Integer);
	}
};

void Lexer::scanDigitPrefixed() {
	char maxValid = getMaxFromPrefix(frontCur.readNext());
	if (maxValid == '\0') return;
	frontCur.fwd();

	while (!isEnd()) {
		char c = frontCur.fwd_read();
		if (!is(c).symCont() && c != '\'') break;
		if (toupper(c) > maxValid)
			handleException("characters from a different number system");
	};
};

void Lexer::scanDigitStandart(bool &isFloat) {
	char c = frontCur.read();
	while (isdigit(c) || c == '\'' || c == '.') {
		if (c == '.')
			isFloat = maybeFloat(isFloat, false);
		c = frontCur.fwd_read();
	};
};

void Lexer::scanDigitExponent() {
	char c = frontCur.fwd_read();

	if (c == '+' || c == '-')
		c = frontCur.fwd_read();
	if (!isdigit(c))
		handleException("expected digits in exponent");

	while (isdigit(c) || c == '\'') {
		c = frontCur.fwd_read();
	};

	if (is(c).symCont())
		handleException("flags for exponential notation is prohibited");
	if (c == '.')
		maybeFloat(true, true); // for the exponent dot exception
};

// Cursor is located before the dot.
// Function checks whether the char after the dot (cursor + 2 chars) is a digit,
// and whether the dot is the first in the number.
bool Lexer::maybeFloat(bool isFloat, bool isExponent) {
	const char next = frontCur.readWithOffset(2);

	if (isEnd(2))
		return false;
	if (isExponent)
		handleException("dot in exponent");
	if (isFloat)
		handleException("multiple decimal dots in the number");

	if (isdigit(next) || next == '\'') {
		if (isExponent)
			handleException("dot in exponent");
		if (isFloat)
			handleException("multiple decimal dots in the number");
		frontCur.fwd(2); // a jump through the dot
		return true;
	} else
		return false;
};

char Lexer::getMaxFromPrefix(char c) {
	if (c == 'x' || c == 'X') return 'F';
	if (c == 'o' || c == 'O') return '7';
	if (c >= '0' && c <= '7') return '7';
	if (c == 'b' || c == 'B') return '1';
	if (c == '8' || c == '9')
		handleException("characters from a different number system");
	return '\0';
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
