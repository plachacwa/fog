#pragma once
#include <string_view>

struct Symbol {
    std::string_view name;
    bool isType;

    Symbol(std::string_view n, bool t) : name(n), isType(t) {};
};
