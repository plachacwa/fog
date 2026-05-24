#pragma once
#include <optional>
#include <utility>
#include <variant>
#include <vector>
#include "parser/operator.h"

struct Type {};

using Literal = std::variant<
    std::monostate,
    std::string_view
>;

struct SymbolInfo {};

struct ASTNode {
    using InfoVariants =
    std::variant<std::monostate, std::optional<Literal>, SymbolInfo*, const Op*>;

    std::monostate        type{};
    InfoVariants          info{};
    std::vector<ASTNode*> children;
	
	explicit ASTNode() = default;

    explicit ASTNode( const InfoVariants &info )
    : info( info ) {};

    explicit ASTNode( std::vector<ASTNode*> children )
    : children( std::move( children ) ) {};

    explicit ASTNode( const InfoVariants &info, std::vector<ASTNode*> children )
    : info( info ), children( std::move( children ) ) {};

    virtual ~ASTNode() = default;
};


struct Atom : public ASTNode {
    explicit Atom( Literal &&literal )
    : ASTNode( literal ) {};
};

struct Symbol : public ASTNode {
    Symbol()
    : ASTNode( static_cast<SymbolInfo*>( nullptr ) ) {};
};

struct Binary : public ASTNode {
    Binary( const Op *op, ASTNode *left, ASTNode *right )
    : ASTNode( op, { left, right } ) {};
};
struct Void : public ASTNode {
	Void() = default;
};