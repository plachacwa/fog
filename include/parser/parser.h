#pragma once
#include <plf_hive.h>
#include <unordered_set>
#include <vector>

#include "optable.h"
#include "common/nodes.h"
#include "common/region.h"
#include "common/token.h"

class Parser {
    using Context = Region;

    std::vector<Token> &        tokens;
    std::size_t                 idx = 0;
    Context &                   ctx;
    plf::hive<std::string_view> errors;
    std::vector<ASTNode*>       ambiguousNodes;

    plf::hive<Literal> literals;
    OpTable            opTable;

    public:
        explicit Parser(std::vector<Token> &tokens, Context &ctx)
        : tokens(tokens), ctx(ctx) {};

        ASTNode* parseExpression(int rbp = -1);

    private:
        bool moveIdx(const std::function<bool()> &);

        bool isTherePhantomSemicolon() const;

        bool isNotEndOfExpression() const;

        ASTNode* parseAtom();

        ASTNode* parseAtomOrUnary();

        ASTNode* parseOperator(ASTNode *, const Op *, bool &, int);

        ASTNode* parseMultiple(ASTNode *, const Op *, bool &, int);

        std::expected<ASTNode*, std::string_view> parseBrackets();

        std::pair<const Op*, std::string_view> opFromCurrentToken(const ASTNode *);
};
