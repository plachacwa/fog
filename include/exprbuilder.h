#pragma once
#include "pcursor.h"
#include "token.h"
#include "ast.h"
#include "globalctx.h"

class ExprBuilder {
	std::vector<uNode> nodes;
	std::vector<Token> operators;
	Parser *parser;
	PCursor &pos;
	
	public:
		ExprBuilder(Parser*);
		uNode parse();
		
		ExprBuilder(const ExprBuilder&) = delete;
		ExprBuilder& operator=(const ExprBuilder&) = delete;
		ExprBuilder(ExprBuilder&&) = delete;
		ExprBuilder& operator=(ExprBuilder&&) = delete;
		
	private:
		bool parseToken();
		bool parseAtom(const Token&);
		bool parseOperator(const Token&);
		
		TypeInfo* getTypeInfo(Token::Type) const;
};