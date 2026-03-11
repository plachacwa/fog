#include "lexer/lexer.h"

#include <cassert>
#include <cmath>
#include <cstring>

#include "lexer/charsets.h"
#include "reader/readstream.h"
#include "lexer/tokenfactory.h"

using namespace std;

Lexer::Lexer(Reader &reader)
    : reader(reader), tf(this) {};

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

    if (c == '\0')               return makeToken(TokenType::End);
    if (isdigit(c))              return scanDigit();
    if (Charset::SymStartExt(c)) return scanSymbol();
    if (c == '\'')               return scanChar();
    if (c == '"')                return scanString();
    if (Charset::Operator(c))    return scanOperator();
	if (Charset::Punct(c))		 return scanPunct();

    return makeToken(TokenType::Unknown);
};

void Lexer::skipWhitespace() {
    tf.maybeMany(Charset::Whitespace).clear();
};



Token Lexer::scanSymbol() {
	return tf.one()
		.maybeMany(Charset::SymContExt)
		.type(TokenType::Symbol)
		.token();
};

// ===============
// NUMBER SCANNING
// ===============

Token Lexer::scanDigit() {
	if (expected maxValid = getMaxFromPrefix(reader.readNextChar());
		reader.readChar() == '0' && maxValid)
		return scanDigitPrefixed(maxValid.value());
	else if (!maxValid)
		tf.pushError(std::move(maxValid.error()));

	scanDigitStandard();

	if (Charset::ExponentPrefix( reader.readChar() ))
		return scanDigitExponent();
	else
		return tf.token();
};

Token Lexer::scanDigitPrefixed(const char maxValid) {
	return tf.one()
			.many(Charset::SymContExt)
			.forEachReaden([&maxValid](const char c) { return toupper(c) <= maxValid; })
			.errorIfFailed("characters from a different number system")
			.type(TokenType::PrefixedInt)
			.token();
}

void Lexer::scanDigitStandard() {
	bool isFloat = false;
	while (!tf.many(Charset::Digit).isFailed) {
		if (reader.readChar() == '.') {
			if (expected floatOrErr = maybeFloat(isFloat); floatOrErr.has_value())
				isFloat = *floatOrErr;
			else {
				tf.pushError(std::move(floatOrErr.error()));
				break;
			}
		} else break;
	};
	tf.ignoreFail().type(isFloat ? TokenType::Float : TokenType::Integer);
};

Token Lexer::scanDigitExponent() {
	tf.one()
		.maybeOne(Charset::PlusMinus)
		.many(Charset::Digit)
		.errorIfFailed("expected digits in exponent");
	if (Charset::SymContExt( reader.readChar() ))
		tf.pushError(error("flags for exponential notation is prohibited"));
	if (reader.readChar() == '.')
		tf.pushError(error("dot in exponent"));
	return tf.type(TokenType::Exponential).token();
};

// Cursor is located at the dot.
// Function checks whether the char after the dot (cursor + 1 char) is a digit,
// and whether the dot is the first dot in the number.
expected<bool, Error> Lexer::maybeFloat(const bool isFloat) const {
	const char next = reader.readNextChar();
	if (Charset::Digit(next)) {
		if (isFloat) return unexpected(error("multiple decimal dots in the number"));
		else		 return true;
	};
	if (Charset::Whitespace(next))
		return unexpected(error("hanging point after a digit is prohibited"));

	return false;
}

expected<char, Error> Lexer::getMaxFromPrefix(char c) const {
	if (c == 'x' || c == 'X') return 'F';
	if (c == 'o' || c == 'O') return '7';
	if (c >= '0' && c <= '7') return '7';
	if (c == 'b' || c == 'B') return '1';
	if (c == '8' || c == '9') return unexpected(
		error("characters from a different number system")
	);
	return '\0';
};

// ===============
// CHAR & STRING SCANNING
// ===============

Token Lexer::scanChar() {
	reader.move(); // skip opening '
	if (reader.readChar() == '\\') {
		if (auto err = processEscSeq())
			tf.pushError(std::move(*err));
	} else
		reader.move();

	if (reader.readChar() != '\'')
		tf.pushError(error("unclosed char literal"));
	reader.move(); // skip closing '
	return tf.type(TokenType::Char).token();
};

Token Lexer::scanString() {
	reader.move(); // skip opening "
	char c = reader.readChar();

	do {
		if (c == '\0')
			tf.pushError(error("unclosed string literal"));
		if (c == '\\')
			if (auto err = processEscSeq())
				tf.pushError(std::move(*err));
		reader.move();
		c = reader.readChar();
	} while (c != '"');
	reader.move(); // skip closing "

	return tf.type(TokenType::String).token();
};

// The function considers that cursor is on the \
// and does not check if this is really the case.
optional<Error> Lexer::processEscSeq() {
	reader.move();
	const char c = reader.readChar();
	int length = 0;
	if (std::ranges::contains("\\'\"nrtbf0e", c))
						length = 1;
	else if (c == 'x')	length = 3;
	else if (c == 'u')	length = 5;
	else if (c == 'U')	length = 9;
	else
		return error("invalid escape sequence");

	reader.move(length);
	return nullopt;
};

Token Lexer::scanOperator() {
	return tf.many(Charset::Operator)
			.type(TokenType::Operator)
			.token();
};

Token Lexer::scanPunct() {
	reader.move();
	return makeToken(TokenType::Punct);
};
/* Comments have that syntax:
 *     -- one-line comment
 *	  {- multiline
 *		 		   comment -}
 */
void Lexer::skipComment() {
	const char c = reader.readChar();
	if (c == '{')
		skipBefore("-}");
	else
		skipBefore("\n");
};

void Lexer::skipBefore(const char* endingSeq) {
	const size_t endingPos = reader.source.find(endingSeq, reader.position.index);
	const size_t length = strlen(endingSeq);

	// If ending sequence is not found, consume rest of input
	if (endingPos == string::npos || endingPos + length > reader.source.length()) {
		tf.maybeMany(Charset::AnyExceptNull);
		return;
	};

	const size_t targetPos = endingPos + length;
	assert(targetPos < std::pow(2, sizeof(int))); // TODO: change mas size of file
	reader.move(static_cast<int>(targetPos) - reader.position.index);
};

Token Lexer::makeToken(TokenType type) const {
	return Token{
		reader.substrFrom(currentPosition),
		type,
		currentPosition.compact()
	};
}

Error Lexer::error(string msg) const {
	return Error {
		currentPosition,
		reader.position.index - currentPosition.index,
		std::move(msg)
	};
};