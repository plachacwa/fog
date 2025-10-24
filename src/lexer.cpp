#include "lexer.h"
#include "utils.h"
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <iostream>
using namespace std;

const unordered_set<string_view>& getKeywords() {
    static const unordered_set<string_view> keywords {
		"break",	"class",	"continue",	"const",
		"else",		"enum",		"extern",	"extend",
		"false",	"final",	"for",		"friend",
		"if",		"import",	"interface","let",
		"match",	"module",	"move",		"private",
		"protected","public",	"return",	"spawn",
		"this",		"true",		"type",		"var",
		"where",	"while",	"yield"
    };
    return keywords;
};

const string_view getOpSymbols() {
    static const string_view opSymbols = "!#$%&*+,-/<=>?@^`|~\\";
    return opSymbols;
};

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
	//cout << "Scan Token!\n";
	backCar.setTo(frontCar);
	const char c = frontCar.read();
	
	if (c == '\0') return makeToken(Token::End);
	//cout << "(X) end\n";
	if (c == '\n') return scanNewLn();
	else
		wasNewLn = false;
	//cout << "(X) newLn\n";
	
	if (isspace(c)) 			return skipSpace();
	//cout << "(X) space\n";
	if (is(c).symStart()) 		return scanSymbol();
	//cout << "(X) symbol\n";
	if (isdigit(c)) 			return scanDigit();
	//cout << "(X) digit\n";
	if (c == '\'')  			return scanChar();
	//cout << "(X) char\n";
	if (c == '"')				return scanString();
	//cout << "(X) string\n";
	
	if (is(c).from("-{") && frontCar.readNext() == '-')
								return scanComment();
	//cout << "(X) comment\n";
	
	if (is(c).from(getOpSymbols()))	return scanOperator();
	//cout << "(X) operator\n";
	if (is(c).from("(){}[];:"))	return scanPunct();
	//cout << "(X) punctuation\n";
	
	frontCar.fwd();
	return makeToken(Token::Unknown);
};

Token Lexer::skipSpace() {
	char c;
	do { c = frontCar.fwd_read(); } while (c != '\n' && isspace(c));
	return scanToken();
};

Token Lexer::scanNewLn() {
	wasNewLn = true;
	frontCar.newLn();
	return scanToken();
};

Token Lexer::scanSymbol() {
	char c;
	do { c = frontCar.fwd_read(); } while (is(c).symCont());
	
	string_view lexeme = getStringBetweenCars();
	
	auto it = getKeywords().find(lexeme);
	
	if (it != getKeywords().end())
		return makeToken(Token::Keyword, lexeme);
	else
		return makeToken(Token::Symbol, lexeme);
};

Token Lexer::scanDigit() {
	char c = frontCar.fwd_read();
	bool isFloat = false;
	while (isdigit(c) || c == '.') {
		if (c == '.')
			isFloat = maybeFloat(isFloat);
		c = frontCar.fwd_read();
	};
	return makeToken(isFloat ? Token::Float : Token::Integer);
};

bool Lexer::maybeFloat(bool isFloat) {
	const char next = frontCar.readNext();
	
	if (isFloat)
		handleException("multiple decimal dots in the number");
	
	if (isdigit(next)) {
		frontCar.fwd();
		return true;
	} else
		return false;
};

Token Lexer::scanChar() {
	frontCar.fwd(); // skip '
	frontCar.fwd(); // skip the letter
	return makeToken(Token::Char);
};

Token Lexer::scanString() {
	frontCar.fwd(); // skip "
	char c;
	do { c = frontCar.fwd_read(); } while (c != '"');
	return makeToken(Token::String);
};

Token Lexer::scanComment() {
	const char c = frontCar.read();
	if (c == '{') {
		skipBefore("-}");
		return scanToken();
	} else {
		skipBefore("\n");
		return scanToken();
	};
};

void Lexer::skipBefore(const char* endingSeq) {
	size_t endingPos = source.find(endingSeq, frontCar.position);
	const size_t length = strlen(endingSeq);
	
	if (endingPos + length >= end) return;
	endingPos += length;
	
	while (frontCar.position != endingPos) {
		if (frontCar.read() == '\n') frontCar.newLn();
		else frontCar.fwd();
	};
};

Token Lexer::scanOperator() {
	char c;
	do { c = frontCar.fwd_read(); } while (is(c).from(getOpSymbols()));
	return makeToken(Token::Operator);
};

Token Lexer::scanPunct() {
	frontCar.fwd();
	return makeToken(Token::Punct);
};


bool Lexer::isEnd(int offset) const {
	return (frontCar.position + offset) >= end;
};
Token Lexer::makeToken(Token::Type tokenType) const {
	return makeToken(tokenType, getStringBetweenCars());
};
Token Lexer::makeToken(Token::Type tokenType, string_view lexeme) const {
	return Token {
		tokenType,
		lexeme, {
			backCar.line,
			backCar.column,
		}, wasNewLn,
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