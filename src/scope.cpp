#include "scope.h"

Scope::Scope(GlobalContext &c, Scope *p) : ctx(c), parent(p) {
	if (!p) {
		c.addType("int");
		c.addType("float");
		c.addType("char");
		c.addType("bool");
	};
};

BaseInfo::Kind Scope::kindOf(std::string_view name) {
	auto it = names.find(name);
	if (it != names.end())
		return it->second->kind;
	else {
		if (parent) return parent->kindOf(name);
		else return BaseInfo::Kind::Undefined;
	};
};

void Scope::push(BaseInfo::Kind kind, std::string_view s, TypeInfo* t) {
	switch(kind) {
		case BaseInfo::Kind::Type:
			names.try_emplace(s, static_cast<BaseInfo*>(ctx.addType(s)));
			break;
		case BaseInfo::Kind::Symbol:
			names.try_emplace(s, static_cast<BaseInfo*>(ctx.addSymbol(s, t)));
			break;
		default:
			break;
	};
};

TypeInfo* Scope::typePtr(std::string_view name) {
	auto it = names.find(name);
	if (it != names.end())
		return static_cast<TypeInfo*>(it->second);
	else {
		if (parent) return parent->typePtr(name);
		else 		return nullptr;
	};
};

Symbol*	Scope::symbolPtr(std::string_view name) {
	auto it = names.find(name);
	if (it != names.end())
		return static_cast<Symbol*>(it->second);
	else {
		if (parent) return parent->symbolPtr(name);
		else 		return nullptr;
	};
};