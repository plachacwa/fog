#include "ast.h"
#include "token.h"

class Parser {
public:
	Parser(std::vector<Token>&);
	
		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;

		Parser(Parser&&) = delete;
		Parser& operator=(Parser&&) = delete;
	
	uNode parse();
private:
	std::vector<Token> &tokens;
	size_t current;

	uNode parseExpression();
};