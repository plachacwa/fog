#include "parser.h"
#include "utils.h"

using namespace std;

Parser::Parser(vector<Token>& t, GlobalContext& ctx) : tokens(t), pos(this, &ctx) { end = tokens.size(); };

uNode Parser::parse() {
	return parseExpression();
};

inline uNode Parser::parseExpression() {
	return ExprBuilder(this).parse();
};

