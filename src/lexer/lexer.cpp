#include "lexer/lexer.h"

#include <cassert>

#include "lexer/charsets.h"
#include "reader/ireader.h"
#include "lexer/tokenfactory.h"

using namespace std;

vector<Token> Lexer::tokenizeAll() {
    vector<Token> tokens;
	tokens.reserve(reader.sourceSize() / 4);
    Token token = nextToken();

    while ( token.type != TokenType::End ) {
        tokens.push_back(token);
        token = nextToken();
    };

    return tokens;
}

Token Lexer::nextToken() {
    const bool hadWhitespace = skipWhitespaceIfExist();
    currentPosition	  = reader.position();
    const Codepoint c = reader.readChar();

    if (c == '\0')             return makeToken(TokenType::End);
	if (c == '\'')             return scanChar();
	if (c == '"')              return scanString();
	if (reader.readNextChar() == '-' && (c == '-' || c == '{'))
		skipComment();
    if (Charset::Digit(c))     return scanDigit();
    if (Charset::SymStart(c))  return scanSymbolOrFlag(!hadWhitespace);
    if (Charset::Operator(c))  return scanOperator();
	if (Charset::Punct(c))	   return scanPunct();

    return makeToken(TokenType::Unknown);
};

bool Lexer::skipWhitespaceIfExist() {
	const bool hadWhitespace =
		!tf.many(Charset::Whitespace).isFailed;
	tf.clear();
	return hadWhitespace;
};

Token Lexer::scanSymbolOrFlag(const bool isFlag) {
	return tf.one()
		.maybeMany(Charset::SymCont)
		.type(isFlag ? TokenType::PossibleFlag : TokenType::Symbol)
		.token();
};

// ===============
// NUMBER SCANNING
// ===============

Token Lexer::scanDigit() {
	if (expected maxValid = getMaxFromPrefix(reader.readNextChar());
		reader.readChar() == '0' && maxValid.has_value() && *maxValid != '\0')
		return scanDigitPrefixed(maxValid.value());
	else if (!maxValid)
		tf.pushError(std::move(maxValid.error()));

	scanDigitStandard();

	if (Charset::ExponentPrefix( reader.readChar() ))
		return scanDigitExponent();
	else
		return tf.token();
};

Token Lexer::scanDigitPrefixed(const Codepoint maxValid) {
	return tf.one()
			.many(Charset::SymCont)
			.forEachReaden([&maxValid](const char c) {
				return static_cast<unsigned int>(toupper(c)) <= maxValid;
			})
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
	if (Charset::SymCont( reader.readChar() ))
		tf.pushError(error("flags for exponential notation is prohibited"));
	if (reader.readChar() == '.')
		tf.pushError(error("dot in exponent"));
	return tf.type(TokenType::Exponential).token();
};

// Cursor is located at the dot.
// Function checks whether the char after the dot (cursor + 1 char) is a digit,
// and whether the dot is the first dot in the number.
expected<bool, Error> Lexer::maybeFloat(const bool isFloat) const {
	const Codepoint next = reader.readNextChar();
	if (Charset::Digit(next)) {
		if (isFloat) return unexpected(error("multiple decimal dots in the number"));
		else		 return true;
	};
	if (Charset::Whitespace(next) || Charset::Punct(next))
		return unexpected(error("hanging point after a digit is prohibited"));

	return false;
}

expected<Codepoint, Error> Lexer::getMaxFromPrefix(Codepoint c) const {
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

	const Charset::Checker InsignificantChars = [](const Codepoint c) {
		return Charset::AnyExceptNull(c) && c != '"' && c != '\\';
	};

	while (true) {
		tf.maybeMany(InsignificantChars);
		const Codepoint c = reader.readChar();

		if (c == '\\')
			if (auto err = processEscSeq())
				tf.pushError(std::move(*err));
		if (c == '\0') {
			tf.pushError(error("unclosed string literal"));
			break;
		};
		if (c == '"') {
			reader.move();
			break;
		};
	};

	return tf.type(TokenType::String).token();
};

// The function considers that cursor is on the \
// and does not check if this is really the case.
optional<Error> Lexer::processEscSeq() {
	reader.move();
	const Codepoint c = reader.readChar();
	int length = 0;

	if (Charset::OneCharEscSeqs(c)) length = 1;
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
 *     {- multiline
 *                comment -}
 */
void Lexer::skipComment() {
	const string_view endingSeq = (reader.readChar() == '{') ? "-}" : "\n";
	reader.move(2);

	const Charset::Checker InsignificantChars = [](const Codepoint c) {
		return Charset::AnyExceptNull(c) && c != '-' && c != '{' && c != '\n';
	};

	while (true) {
		tf.maybeMany(InsignificantChars);
		const char c = reader.readUtf8Char().ascii();

		if (c == '{') skipComment();
		if (c == '\0') return;
		if (c == endingSeq[0]) {
			const string_view possibleSeq =
				string_view(reader.readUtf8Char().ptr())
				.substr(0, endingSeq.length());
			if (possibleSeq == endingSeq) return;
		};
		reader.move();
	}
};

Token Lexer::makeToken(const TokenType type) const {
	return Token {
		reader.substrFrom(currentPosition),
		type,
		currentPosition.compact()
	};
}

Error Lexer::error(string msg) const {
	return Error {
		currentPosition,
		reader.position().index - currentPosition.index,
		std::move(msg)
	};
};