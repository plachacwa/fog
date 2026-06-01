#pragma once
#include <string>

#include "token.h"

struct Highlighting {
    BigPosition start;
    int length;
};

struct Error {
    Highlighting strongHl;
    Highlighting weakHl;
    std::string msg;
};
