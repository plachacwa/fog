#pragma once
#include "basenode.h"

// Value nodes

struct Void : public ASTNode {
    Void() = default;

    [[nodiscard]] const char* name() const override { return "Void"; };
    [[nodiscard]] std::string infoStr() const override;
};

struct Atom : public ASTNode {
    explicit Atom(Literal &&literal) : ASTNode(literal) {};

    [[nodiscard]] const char* name() const override { return "Atom"; };
    [[nodiscard]] std::string infoStr() const override;
};

struct Symbolic : public ASTNode {
    explicit Symbolic(SymbolInfo *info) : ASTNode(info) {};

    [[nodiscard]] const char* name() const override { return "Symbol"; };
    [[nodiscard]] std::string infoStr() const override;
};

// Expression nodes
struct IOperator : public ASTNode {
    template <class ...Args>
    explicit IOperator(const Op *op, Args ...args) : ASTNode(op, { args ... }) {};

    ~IOperator() override = default;

    [[nodiscard]] std::string infoStr() const override;
};

struct OperatorNode : public IOperator {
    explicit OperatorNode(const Op* op) : IOperator(op) {};

    [[nodiscard]] const char* name() const override { return "OperatorNode"; };
};

struct Unary : public IOperator {
    Unary(const Op *op, ASTNode *node) : IOperator(op, node) {};

    [[nodiscard]] const char* name() const override { return "Unary"; };
};

struct Binary : public IOperator {
    Binary(const Op *op, ASTNode *left, ASTNode *right) : IOperator(op, left, right) {};

    [[nodiscard]] const char* name() const override { return "Binary"; };
};

struct Multiple : public IOperator {
    template <class ...Args>
    explicit Multiple(const Op *op, Args ...args) : IOperator(op, args ...) {};

    [[nodiscard]] const char* name() const override { return "Multiple"; };
};

struct Definition : public IOperator {
    explicit Definition(const Op *op, ASTNode *node) : IOperator(op, node) {};

    [[nodiscard]] const char* name() const override { return "Definition"; };
};

// Function node
struct Function : public ASTNode {
    explicit Function(Symbolic *name) {};
};



struct HighLevelStmt {};

struct Directive : public ASTNode, HighLevelStmt {
    
};

struct File : public ASTNode {
    explicit File(HighLevelStmt*);
};