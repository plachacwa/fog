#include "lexer/lexer.h"

#include "lexer/charsets.h"
#include "reader/readstream.h"
using namespace std;

Lexer::Lexer(Reader &reader)
    : reader(reader), currentPosition() {};

vector<Token> Lexer::tokenizeAll() {
    vector<Token> tokens;
    Token token = nextToken();

    while ( token.type != TokenType::End ) {
        tokens.push_back(token);
        token = nextToken();
    };

    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    currentPosition = reader.position;
    const char c    = reader.readChar();

    if (c == '\0')              return makeToken(TokenType::End);
    if (isdigit(c))             return scanDigit();
    if (Charset::SymStartExt(c))   return scanSymbol();
    if (c == '\'')              return scanChar();
    if (c == '"')               return scanString();
    if (Charset::Operator(c))   return scanOperator();

    return makeToken(TokenType::Unknown);
};

void Lexer::skipWhitespace() const {
    ReadStream(&reader)
        .maybeMany(Charset::Whitespace)
		.move();
};



Token Lexer::scanSymbol() {
	reader.move();
	ReadStream(&reader)
		.many(Charset::SymContExt)
		.move();

	const string_view lexeme = reader.substrFrom(currentPosition);

	return makeToken(TokenType::Symbol, lexeme);
};

// ===============
// NUMBER SCANNING
// ===============

Token Lexer::scanDigit() {
	if (reader.readChar() == '0' && getMaxFromPrefix(reader.readNextChar())) {
		scanDigitPrefixed();
		return makeToken(TokenType::PrefixedInt);
	};

	bool isFloat = false;
	scanDigitStandard(isFloat);
	if (reader.readChar() == 'e' || reader.readChar() == 'E') {
		scanDigitExponent();
		return makeToken(TokenType::Exponential);
	};
	return makeToken(isFloat ? TokenType::Float : TokenType::Integer);
};

void Lexer::scanDigitPrefixed() {
	char maxValid = getMaxFromPrefix(reader.readNextChar());
	if (maxValid == '\0') return;
	reader.move();

	/*while (!isEnd()) {
		char c = frontCur.fwd_read();
		if (!is(c).symCont() && c != '\'') break;
		if (toupper(c) > maxValid)
			handleException("characters from a different number system");
	};*/
	ReadStream(&reader)
		.many(Charset::SymCont);
};

void Lexer::scanDigitStandard(bool &isFloat) {
	char c = reader.readChar();
	while (isdigit(c) || c == '\'' || c == '.') {
		if (c == '.')
			isFloat = maybeFloat(isFloat, false);
		c = reader.fwd_read();
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

Token Lexer::makeToken(TokenType type) {
	return makeToken(type, reader.substrFrom(currentPosition));
};

Token Lexer::makeToken(TokenType type, string_view lexeme) {
	return Token{
		lexeme,
		type,
		currentPosition
	};
};