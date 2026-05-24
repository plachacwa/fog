#pragma once
#include <string>
#include <unordered_set>
#include <vector>

#include "typesets.h"
#include "common/astnode.h"
#include "common/region.h"
#include "common/token.h"
#include "optable.h"

class Parser {
    using Context = Region;

    std::vector<Token> &tokens;
    std::size_t idx = 0;
    Context &ctx;

    std::unordered_set<std::string_view> stringLiterals;
    OpTable opTable;

    public:
        explicit Parser(std::vector<Token> &tokens, Context &ctx)
        : tokens(tokens), ctx(ctx) {};

        ASTNode* parseExpression( int priority = -1 );
    
    private:
		bool moveIdx();
		bool isNotEndOfExpression();
		
		ASTNode* parseAtom();
		ASTNode* parseAtomOrUnary();
		std::pair<const Op*, bool> binaryOpFromCurrentToken( const ASTNode* );
	
};
