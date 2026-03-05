#include "parser/opreg.h"
using namespace std;

OperatorRegister::OperatorRegister(Region &c)
    : ctx(c), ops(), precendenceList() {};

OperatorInfo* OperatorRegister::findOperator(string_view glyph) {
    auto it = ops.find(glyph);
    return it != ops.end()
         ? it->second
         : nullptr;
};


int OperatorRegister::getPrecedenceFor(OperatorInfo* op) {
    for (int i = 0; i < precendenceList.size(); i++) {
        auto it = precendenceList[i].find(op);
        if (it != precendenceList[i].end())
            return i;
    };
    return -1;
};
