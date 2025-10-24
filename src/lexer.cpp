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

static constexpr string_view opSymbols = "!#$%&*+,-/<=>?@^`|~\\";


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
	char c = frontCar.fwd_read();
	bool isFloat = false;
	while (isdigit(c)) {
		if (frontCar.readNext() == '.')
			isFloat = maybeFloat(isFloat);
		c = frontCar.fwd_read();
	};
	return makeToken(isFloat ? Token::Float : Token::Integer);
};

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

// TODO: Escape-sequences
Token Lexer::scanChar() {
	frontCar.fwd(); // skip '
	frontCar.fwd(); // skip the letter
	return makeToken(Token::Char);
};
// TODO: Escape-sequences
Token Lexer::scanString() {
	frontCar.fwd(); // skip "
	char c;
	do { c = frontCar.fwd_read(); } while (!isEnd() && c != '"');
	return makeToken(Token::String);
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




void Lexer::skipSpace() {
	char c;
	do { c = frontCar.fwd_read(); } while (c != '\n' && isspace(c));
};

void Lexer::processNewLn() {
	wasNewLn = true;
	frontCar.newLn();
};
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
	
	if (endingPos == string::npos || endingPos + length > end) {
		while (frontCar.position < end)
			frontCar.fwd();
		return;
	};
	
	const size_t targetPos = endingPos + length;
	while (frontCar.position != targetPos) {
		if (frontCar.read() == '\n') frontCar.newLn();
		else frontCar.fwd();
	};
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

void Lexer::handleException(string msg) {
	const string
		line = to_string(backCar.line),
		column = to_string(backCar.column);
	throw runtime_error(
			"Lexer ERROR: " + msg + " at line " + line + ", column " + column
		);
};