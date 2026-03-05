#pragma once
#include <string_view>
#include "enums.h"

struct OperatorRegister;

struct OperatorInfo {
    using Kind          = OperatorEnums::Kind;
    using Associativity = OperatorEnums::Associativity;
    using enum ::OperatorEnums::Associativity;
    using enum ::OperatorEnums::Kind;

    /* members */
    std::string_view glyph;
    Kind kind;
    Associativity associativity;

    /*public:
        //int precendence() { return mng.getPrecendence(this); };
        int precendenceFrom(OperatorRegister *mng) { return mng.getPrecendenceFor(this); };*/
};
