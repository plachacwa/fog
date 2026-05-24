#include <vector>
#include <string>
#include "parser/parser.h"
#include "parser/optable.h"

using namespace std;

bool Parser::moveIdx(std::function<bool()> checker = nullptr) {
    const bool check = idx+1 < tokens.size();
    if (checker) check &&= checker();
    idx += check;
    return check;
};

ASTNode* Parser::parseAtom() {
    const auto &t = tokens[idx];
    moveIdx();
    stringLiterals.emplace( t.lexeme );

    return Typeset::isValueOrSymbol( t )
         ? ctx.make<Atom>(t.lexeme)
         : nullptr;
};

ASTNode* Parser::parseExpression(const int rbp) {
	auto left = parseAtomOrUnary();
	while (isNotEndOfExpression()) {
		const auto [op, opLexeme]  = binaryOpFromCurrentToken(left);
		if (const auto lbp = opTable.getOpPriority(op); lbp.has_value() && *lbp > rbp) {
			if (opLexeme != " ")
				moveIdx();
			const int power = (opLexeme == "[]")? -1 : *lbp - (op->arity == Op::RightAssociative);
			left = ctx.make<Binary>( op, left, parseExpression(power) ));
			if (opLexeme == "[]") {
				if (tokens[idx].type != TokenType::RSB)
					assert(false && "expected `]`");
				moveIdx();  // passing ]
			};
		};
		else break;
	};
	return left;
};

pair<const Op*, string_view> Parser::binaryOpFromCurrentToken(const ASTNode* left) {
	const auto &t = tokens[idx];
	string_view lexeme{};
	
	if (t.type == TokenType::Operator)   lexeme = t.lexeme;
	else if (t.type == TokenType::LSB)   lexeme = "[]";
	else if (Typeset::isValueOrSymbol(t)
		  && isTypeOrFunction(left))     lexeme = " ";
	
	return { 
		opTable.find(lexeme, Op::Binary), // will return nullptr if lexeme is empty
		lexeme
	};
};

bool Parser::isNotEndOfExpression() {
	const auto &t = tokens[idx];
	return Typeset::isValueOrSymbol(t)
		|| Typeset::isLeftBracket(t)
		|| t.type == TokenType::Operator;
};

ASTNode* Parser::parseAtomOrUnary() {
	const auto &t = tokens[idx];
	if (t.type == TokenType::Operator) {
		const Op *op = opTable.find(t.lexeme, Op::UnaryPrefix);
		if (!op)
			return nullptr;
		moveIdx();
		return parseExpression(opTable.getOpPriority(op));
	} else if (Typeset::isLeftBracket(t)) {
		moveIdx();
		const auto parsedExpr = parseExpression(-1);
		if (static_cast<int>(tokens[idx].type) - static_cast<int>(t.type) != 1)
			assert(false && "mismatched brackets");
		moveIdx(); // consuming right bracket
		return parsedExpr;
	} else return parseAtom();
};