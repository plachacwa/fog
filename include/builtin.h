#pragma once
#include <vector>

#include "parser/operator.h"

namespace Builtin {
    using enum Op::Associativity;
    using enum Op::Arity;

    const std::vector<std::vector<Op>> basicOperators = {
        {
            { "+", LeftAssociative, Binary },
            { "+", NonAssociative,  UnaryPrefix },
            { "-", LeftAssociative, Binary },
            { "-", NonAssociative,  UnaryPrefix },
        },
    };
    const int basicOperatorsCount = []( const std::vector<std::vector<Op>> &operators ){
        int size = 0;
        for ( const auto &vec : operators )
            size += vec.size();
        return size;
    }( basicOperators );
};