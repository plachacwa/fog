#pragma once
#include <string_view>

struct BaseInfo {
	enum class Kind { Type, Symbol, Literal, Undefined };
	Kind kind;
};

struct TypeInfo : BaseInfo {
    std::string_view name;
	
	TypeInfo(std::string_view n)
		: BaseInfo{Kind::Type}, name(n) {};
};
struct Symbol : BaseInfo {
    std::string_view name;
    TypeInfo* type;
	
	Symbol(std::string_view n, TypeInfo* t)
		: BaseInfo{Kind::Symbol}, name(n), type(t) {};
};
struct Literal : BaseInfo {
    std::string_view value;
    TypeInfo* type;
	
	Literal(std::string_view v, TypeInfo* t)
		: BaseInfo{Kind::Literal}, value(v), type(t) {};
};