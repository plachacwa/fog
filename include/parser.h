#include "ast.h"
#include "token.h"
#include "exprbuilder.h"
#include "pcursor.h"

class Parser {
	std::vector<Token> &tokens;
	PCursor pos;
	size_t end;
	
	public:
		Parser(std::vector<Token>&, GlobalContext&);
	
		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;

		Parser(Parser&&) = delete;
		Parser& operator=(Parser&&) = delete;
	
		uNode parse();
	private:
		friend Token& PCursor::readWithOffset(int) const;
		friend bool PCursor::isEnd(int) const;
	
		inline uNode parseExpression();
		
		inline std::unique_ptr<Atom> makeAtom(std::string_view);
		
		friend ExprBuilder::ExprBuilder(Parser*);
};