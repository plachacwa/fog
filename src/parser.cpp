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

Parser::Parser(vector<Token>& t) : tokens(t), current(0) {};

uNode Parser::parse() {
	return parseExpression();
};

uNode Parser::parseExpression() {
	return uNode();
};