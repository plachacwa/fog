#include "pcursor.h"
#include "token.h"
#include "ast.h"

class ExprBuilder {
public:
	ExprBuilder(PCursor&);
	uNode parse();
private:
	std::vector<uNode> nodes;
	std::vector<Token> operators;
	PCursor &pos;
	
	bool parseToken();
	
public:
	ExprBuilder(const ExprBuilder&) = delete;
	ExprBuilder& operator=(const ExprBuilder&) = delete;
	
	ExprBuilder(ExprBuilder&&) = delete;
	ExprBuilder& operator=(ExprBuilder&&) = delete;
};