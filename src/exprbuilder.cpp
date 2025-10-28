#include "exprbuilder.h"
using namespace std;

ExprBuilder::ExprBuilder(PCursor& p) : nodes(), operators(), pos(p) {};

uNode ExprBuilder::parse() {
	while (!pos.isEnd() && parseToken());
	return std::move(nodes.front());
};

bool ExprBuilder::parseToken() {
	Token& t = pos.read();
	bool isEndOfExpression = true;
	string_view ltype;
	
	pos.fwd();
	switch(t.type) {
		case Token::Integer: ltype = "int";		break;
		case Token::Float:	 ltype = "float";	break;
		case Token::Char:	 ltype = "char";	break;
		case Token::String:	 ltype = "str";		break;
		case Token::End:	 ltype = "";		break;
		default:			 ltype = "unknown";	break;
	};
	nodes.push_back(make_unique<Atom>(t.lexeme, ltype));
	return isEndOfExpression;
};