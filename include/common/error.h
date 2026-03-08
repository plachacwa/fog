#pragma once
#include <string>

#include "token.h"

struct Error {
    Position position;
    int length;
    std::string msg;
};
