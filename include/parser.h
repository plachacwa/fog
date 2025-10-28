#include "ast.h"
#include "token.h"
#include "exprbuilder.h"
#include "pcursor.h"

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
	PCursor pos;
	size_t end;
	
	friend Token& PCursor::readWithOffset(int) const;
	friend bool PCursor::isEnd(int) const;

	inline uNode parseExpression();
	
	inline std::unique_ptr<Atom> makeAtom(std::string_view);
};