#pragma once
#include "reader.h"
#include <expected>

struct ReadStream {
    Reader *reader;
    int offset    = 0;
    bool isFailed = false;
    bool isEnd    = false;

    using Checker = bool (*)(char);

    public:
        explicit ReadStream(Reader*);

        ReadStream one      (Checker);
        ReadStream many     (Checker);
        ReadStream maybeOne (Checker);
        ReadStream maybeMany(Checker);

        ReadStream move();
};