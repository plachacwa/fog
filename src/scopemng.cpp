#include "scopemng.h"
#include <cassert>
using namespace std;

ScopeManager::ScopeManager(GlobalContext &c) : scopes(), ctx(c) {
    newScope();
};

void ScopeManager::newScope() { scopes.push_back({}); };
void ScopeManager::deleteScope() {
    assert(scopes.size() <= 1 && "Attempt to delete the root scope.");
    scopes.pop_back();
};

Symbol* ScopeManager::getSymbol(string_view name) {
    Symbol* result = nullptr;
    for (auto curScope = scopes.rbegin(); curScope != scopes.rend(); ++curScope)
        if (result = findInScope(*curScope, name)) break;
    return result;
};

Symbol* ScopeManager::findInScope(const Scope &currentScope, string_view name) {
    auto it = currentScope.find(name);
    if (it != currentScope.end())
        return it->second;
    return nullptr;
};
