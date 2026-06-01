#include <expected>
#include <string>
#include <vector>

#include "parser/parser.h"
#include "common/error.h"
#include "parser/optable.h"
#include "parser/typesets.h"
#include "utils/converter.h"

using namespace std;
using namespace std::literals;

Literal parsePrefixedInt(const string_view lexeme) {
    const auto number = string(lexeme);
    const char prefix = tolower(number[1]);
    char       base   = 0;

    if (prefix == 'x') base = 16;
    if (prefix == 'b') base = 2;
    if (prefix == 'o' || std::isdigit(prefix)) base = 8;

    return std::stoi(number.substr(2), nullptr, base);
};

Literal parseChar(const string_view number) {
    if (number.size() == 3)
        return number[1];
    else if (number.size() == 4) {
        return convertByTable<char, char32_t, 9>(
            {
                pair{ 'n', 0x000A },
                pair{ 'r', 0x000D },
                pair{ 't', 0x0009 },
                pair{ '0', 0x0000 },
                pair{ 'b', 0x0008 },
                pair{ 'e', 0x001B },
                pair{ '"', '"' },
                pair{ '\'', '\'' },
                pair{ '\\', '\\' }
            }, number[2]
        ).value();
    } else {
        const auto codeLength = convertByTable<char, int, 3>(
            {
                pair{ 'x', 2 },
                pair{ 'u', 4 },
                pair{ 'U', 8 }
            }, number[2]
        ).value();
        return std::stoi(string(number.substr(3, codeLength)));
    };
};
Literal parseInt(const string_view number) { return std::stoi(string(number)); };
Literal parseFloat(const string_view number) { return std::stof(string(number)); };

Literal parseString(const string_view lexeme) {
    return lexeme.substr(1, lexeme.size() - 2);
};

bool Parser::moveIdx(const std::function<bool()> &checker = nullptr) {
    if (idx + 1 >= tokens.size()) return false;
    if (checker && !checker()) return false;
    idx++;
    return true;
};

bool Parser::isTherePhantomSemicolon() const {
    const bool isLCB       = tokens[idx].type == TokenType::LCB;
    const bool isHangingOp = tokens[idx - 1].type == TokenType::Operator;
    const int  diff        = tokens[idx].position.line - tokens[idx - 1].position.line;
    return diff > 2 - 2*isLCB + isHangingOp;
};

ASTNode* Parser::parseAtom() {
    const auto &t = tokens[idx];
    moveIdx();

    using TT          = TokenType;
    using LTSV        = Literal(*)(string_view);
    Literal atomValue =
            convertByTable<TokenType, Literal(*)(string_view), 6>({
                pair<TT, LTSV>{ TT::Integer, parseInt },
                pair<TT, LTSV>{ TT::Float, parseFloat },
                pair<TT, LTSV>{ TT::PrefixedInt, parsePrefixedInt },
                pair<TT, LTSV>{ TT::Exponential, parseFloat },
                pair<TT, LTSV>{ TT::Char, parseChar },
                pair<TT, LTSV>{ TT::String, parseString }
            }, t.type).value()(t.lexeme);

    return Typeset::isValueOrSymbol(t)
           ? ctx.make<Atom>(std::move(atomValue))
           : nullptr;
};

ASTNode* Parser::parseExpression(const int rbp) {
    ASTNode *left       = parseAtomOrUnary();
    bool     isMultiple = false;
    while (isNotEndOfExpression()) {
        const auto [op, opLexeme] = opFromCurrentToken(left);
        if (const auto lbp = opTable.getOpPriority(op); lbp.has_value() && *lbp > rbp)
            left = parseOperator(left, op, isMultiple, *lbp);
        else {
            if (!lbp.has_value())
                errors.emplace("unknown operator");
            break;
        };
    };
    return left;
};

ASTNode* Parser::parseOperator(ASTNode *left, const Op *op, bool &isMultiple, const int lbp) {
    if (isTherePhantomSemicolon())
        return left;

    if (op->lexeme == "[]"sv)
        return ctx.make<Binary>(op, left, parseBrackets().value());
    if (op->arity == Op::Multiple)
        return parseMultiple(left, op, isMultiple, lbp);

    if (op->lexeme != " "sv)
        moveIdx();
    const int power = lbp - (opTable.getOpAssociativity(op) == Op::RightAssociative);
    return ctx.make<Binary>(op, left, parseExpression(power));
};

ASTNode* Parser::parseMultiple(ASTNode *left, const Op *op, bool &isMultiple, const int lbp) {
    moveIdx();
    const auto leftOp = left->thisIs<Multiple>();
    if (!isMultiple || leftOp->infoStr() != op->lexeme) {
        left       = ctx.make<Multiple>(op, left, parseExpression(lbp));
        isMultiple = true;
    } else
        left->children.push_back(parseExpression(lbp));
    return left;
};

pair<const Op*, string_view> Parser::opFromCurrentToken(const ASTNode *left) {
    const auto &t = tokens[idx];
    string_view lexeme{};

    if (t.type == TokenType::Operator) lexeme = t.lexeme;
    else if (t.type == TokenType::LSB) lexeme = "[]";
        //	else if (Typeset::isValueOrSymbol(t)
        //		  && isTypeOrFunction(left))     lexeme = " ";
    else if (Typeset::isValueOrSymbol(t) && left->thisIs<Symbolic>()) lexeme = " "; //TEMP

    return { // will return nullptr if lexeme is empty
        opTable.find(lexeme, Op::Binary)
        ? : opTable.find(lexeme, Op::Multiple),
        lexeme
    };
};

bool Parser::isNotEndOfExpression() const {
    const auto &t = tokens[idx];
    return Typeset::isValueOrSymbol(t)
            || Typeset::isLeftBracket(t)
            || t.type == TokenType::Operator;
};

ASTNode* Parser::parseAtomOrUnary() {
    if (const auto &t = tokens[idx]; t.type == TokenType::Operator) {
        const Op *op = opTable.find(t.lexeme, Op::UnaryPrefix);
        if (!op)
            return nullptr;
        if (isTherePhantomSemicolon())
            return ctx.make<OperatorNode>(op);
        moveIdx();
        return ctx.make<Unary>(op, parseExpression(opTable.getOpPriority(op).value()));
    } else if (Typeset::isLeftBracket(t))
        return parseBrackets().value();
    else
        return parseAtom();
};

expected<ASTNode*, string_view> Parser::parseBrackets() {
    const auto &leftBracket = tokens[idx];
    moveIdx();
    if (leftBracket.type != TokenType::LCB && isTherePhantomSemicolon())
        return unexpected("lack of closing bracket");
    const auto  parsedExpr   = parseExpression(-1);
    const auto &rightBracket = tokens[idx];
    if (static_cast<int>(rightBracket.type) - static_cast<int>(leftBracket.type) != 1)
        return unexpected("lack of closing bracket");
    moveIdx();
    return parsedExpr;
};
