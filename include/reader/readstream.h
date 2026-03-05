#include "reader.h"

struct ReadStream {
    Reader *reader;
    int offset    = 0;
    bool isFailed = false;
    bool isEnd    = false;

    using Checker = bool (*)(char);

    public:
        explicit ReadStream(Reader*);

        ReadStream oneFrom      (Checker);
        ReadStream manyFrom     (Checker);
        ReadStream maybeOneFrom (Checker);
        ReadStream maybeManyFrom(Checker);
};