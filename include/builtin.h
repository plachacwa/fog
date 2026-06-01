#pragma once
#include <vector>

#include "parser/operator.h"

namespace Builtin {
    using enum Op::Associativity;
    using enum Op::Arity;

    const std::vector<std::pair<Op::Associativity, std::vector<Op>>> basicOperators = {
        {EqAssociative, {
            { ",", Multiple }
        }},
        {LeftAssociative, {
            { "+", Binary },
            { "+", UnaryPrefix },
            { "-", Binary },
            { "-", UnaryPrefix },
        }},
        {LeftAssociative, {
            { "*", Binary },
            { "/", Binary },
        }},
    };
    const int basicOperatorsCount = []( const auto &operators ){
        int size = 0;
        for ( const auto &pair : operators )
            size += pair.second.size();
        return size;
    }( basicOperators );
};