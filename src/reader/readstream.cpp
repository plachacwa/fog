#include "reader/readstream.h"

ReadStream::ReadStream(Reader* reader) : reader(reader) {};

ReadStream ReadStream::one(Checker isFromCharset) {
    if (isFailed) return *this;

    const int beginOffset = offset;
    maybeOne(isFromCharset);
    if (beginOffset == offset)
        isFailed = true;
    return *this;
};

ReadStream ReadStream::many(Checker isFromCharset) {
    if (isFailed) return *this;

    bool isEmpty = true;
    while (true) {
        one(isFromCharset);
        if (isFailed) {
            isFailed = isEmpty;
            break;
        };
        isEmpty = false;
    };
    return *this;
};

ReadStream ReadStream::maybeOne(Checker isFromCharset) {
    if (isFailed) return *this;
    if (const char c = reader->readCharWithOffset(offset)) {
        if (isFromCharset(c)) offset++;
    } else isEnd = true;
    return *this;
};

ReadStream ReadStream::maybeMany(Checker isFromCharset) {
    if (isFailed) return *this;
    many(isFromCharset);
    if (reader->readChar() == '\0')
        isEnd = true;
    return *this;
}

ReadStream ReadStream::move() {
    reader->move(offset);
    offset = 0;
    return *this;
}
