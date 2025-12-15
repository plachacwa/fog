#pragma once
#include <unordered_map>
#include <string_view>
#include "globalctx.h"

class Scope {
	GlobalContext& ctx;
	Scope* parent;
	std::unordered_map<std::string_view, BaseInfo*> names;
	
	public:
		Scope(GlobalContext&, Scope* = nullptr);
		
		BaseInfo::Kind kindOf(std::string_view);
		
		void push(BaseInfo::Kind, std::string_view, TypeInfo* = nullptr);
		
		TypeInfo* typePtr(std::string_view);
		Symbol*   symbolPtr(std::string_view);
};