#pragma once
#include <string>

#include "token.h"

struct Error {
    BigPosition position;
    int length;
    std::string msg;
};
