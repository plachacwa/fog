#pragma once
#include "infostructs.h"
#include "region.h"
#include <unordered_map>
#include <string_view>

struct GlobalContext {
	Region region;
	Scope parentScope;
	std::unordered_map<std::string_view, Literal*> literals;
	
	GlobalContext() {};
	
	TypeInfo* addType(std::string_view type) {
		return region.make<TypeInfo>(type);
	};
	Symbol* addSymbol(std::string_view name, TypeInfo* type) {
		return region.make<Symbol>(name, type);
	};
	Literal* addLiteral(std::string_view value, TypeInfo* type) {
		auto it = literals.find(value);
		if (it != literals.end())
			return it->second;
		else {
			auto ptr = region.make<Literal>(value, type);
			literals.emplace(value, ptr);
			return ptr;
		};			
	};
	
	Literal* getLiteral(std::string_view value) {
		auto it = literals.find(value);
		if (it != literals.end())
			return it->second;
		else
			return nullptr;
	};
};