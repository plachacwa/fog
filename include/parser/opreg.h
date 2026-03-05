#pragma once
#include "common/operators.h"
#include "common/region.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string_view>

class OperatorRegister {
    Region &ctx;
    std::unordered_map<std::string_view, OperatorInfo*> ops;
    std::vector<std::unordered_set<OperatorInfo*>> precendenceList;

    public:
        OperatorRegister(Region&);

        OperatorInfo* findOperator(std::string_view);

        template <class ...Args>
        OperatorInfo* addOperator(std::string_view glyph, Args&& ...args) {
            if (findOperator(glyph) != nullptr)
                return nullptr;

            OperatorInfo* ptr = ctx.make<OperatorInfo>(glyph, std::forward<Args>(args)...);
            ops[glyph] = ptr;
            return ptr;
        };
    private:
        int getPrecedenceFor(OperatorInfo*);
};
