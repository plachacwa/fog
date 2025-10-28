#include "parser.h"
#include "utils.h"
#include <array>
using namespace std;

static constexpr
array<Token::Type, 5> valueTypes = {
	Token::Symbol,
	Token::Integer,
	Token::Float,
	Token::Char,
	Token::String,
};

Parser::Parser(vector<Token>& t) : tokens(t), pos(this) { end = tokens.size(); };

uNode Parser::parse() {
	return parseExpression();
};

inline uNode Parser::parseExpression() {
	return ExprBuilder(pos).parse();
};

