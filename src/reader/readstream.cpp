#include "reader/readstream.h"

ReadStream::ReadStream(Reader* reader) : reader(reader) {};

ReadStream ReadStream::oneFrom(Checker isFromCharset) {
    if (isFailed) return *this;

    const int beginOffset = offset;
    maybeOneFrom(isFromCharset);
    if (beginOffset == offset)
        isFailed = true;
    return *this;
};

ReadStream ReadStream::manyFrom(Checker isFromCharset) {
    if (isFailed) return *this;

    bool isEmpty = true;
    while (true) {
        oneFrom(isFromCharset);
        if (isFailed) {
            isFailed = isEmpty;
            break;
        };
        isEmpty = false;
    };
    return *this;
};

ReadStream ReadStream::maybeOneFrom(Checker isFromCharset) {
    if (isFailed) return *this;
    if (const char c = reader->readCharWithOffset(offset)) {
        if (isFromCharset(c)) offset++;
    } else isEnd = true;
    return *this;
};

ReadStream ReadStream::maybeManyFrom(Checker isFromCharset) {
    if (isFailed) return *this;
    manyFrom(isFromCharset);
    if (reader->readChar() == '\0')
        isEnd = true;
    return *this;
}