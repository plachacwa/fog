#pragma once
#include <deque>
#include <unordered_map>
#include <string_view>
#include "symbol.h"
#include "globalctx.h"

using Scope = std::unordered_map<std::string_view, Symbol*>;

class ScopeManager {
    std::deque<Scope> scopes;
    GlobalContext &ctx;

    public:
        ScopeManager(GlobalContext&);
        void newScope();
        void deleteScope();
        Symbol* getSymbol(std::string_view);

        template <class ...Args>
        Symbol* addSymbol(std::string_view name, Args&& ...args) {
            Scope &currentScope = scopes.back();

            // if an symbol is already exist in the current scope
            if (findInScope(currentScope, name) != nullptr)
                return nullptr;

            Symbol* symbolPtr = ctx.add<Symbol>(name, std::forward<Args>(args)...);
            currentScope[name] = symbolPtr;
            return symbolPtr;
        };
    private:
        Symbol* findInScope(const Scope&, std::string_view);
};
