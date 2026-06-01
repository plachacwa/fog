#include "common/nodes.h"
#include <array>
#include <iostream>

#include "../../include/common/basenode.h"
#include "utils/interval.h"
using namespace std;

void ASTNode::print(const int indent) const {
    for (int i = 0; i < indent; i++)
        cout << ' ';
    cout << name() << ": " << infoStr() << '\n';
    for (auto &n : children)
        n->print(indent+4);
};

string Atom::infoStr() const {
    const auto literal = get<Literal>(info);
    return visit([&]<typename T0>(const T0& val) -> string {
        using T = decay_t<T0>;
        if constexpr (is_same_v<T, string_view>)
            return string(val);
        else if constexpr (is_same_v<T, char32_t>)
            return "U+" + to_string(static_cast<unsigned int>(val));
        else
            return to_string(val);  // int, double
        return "";
    }, literal);
};
string IOperator::infoStr() const {
    return get<const Op*>(info)->lexeme;
};