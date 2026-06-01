#pragma once
#include <vector>
#include <variant>

struct Symbol;
struct Namespace;
struct Scope;

struct Symbol {
    std::variant<Namespace*, Scope*> parent;
    Namespace* childNamespace;
    Scope* childScope;
};

struct Scope {
    std::variant<Scope*, Symbol*> parent;
    std::vector<Symbol*> symbols;
};

struct Namespace {
    Namespace* parent;
    Symbol* correspondSymbol;
    std::vector<Symbol*> symbols;
};