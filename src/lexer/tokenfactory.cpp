#include <utility>

#include "lexer/tokenfactory.h"

#include "lexer/lexer.h"
#include "lexer/charsets.h"
#include "reader/ireader.h"

using namespace std;

TokenFactory::TokenFactory(Lexer *lexer) : lexer(lexer) {
    t.position = lexer->currentPosition;
};

TokenFactory& TokenFactory::clear() {
    t = {};
    isFailed = false;
    return *this;
}

TokenFactory& TokenFactory::one(const Checker& charsetHas, optional<Error> &&ifNot) {
    return matchingTemplate(
        charsetHas,
        [this]{ lexer->reader.move(); },
        true,
        std::move(ifNot)
    );
};
TokenFactory& TokenFactory::many(const Checker& charsetHas, optional<Error> &&ifNot) {
    return matchingTemplate(
        charsetHas,
        [this, charsetHas]{
            while (charsetHas( lexer->reader.readChar() )) lexer->reader.move();
         }, true,
        std::move(ifNot)
    );
};
TokenFactory& TokenFactory::maybeOne(const Checker& charsetHas) {
    return matchingTemplate(
        charsetHas,
        [this]{ lexer->reader.move(); },
        false
    );
};
TokenFactory& TokenFactory::maybeMany(const Checker& charsetHas) {
    return matchingTemplate(
        charsetHas,
        [this, charsetHas]{
            while (charsetHas( lexer->reader.readChar() )) lexer->reader.move();
         }, false
    );
};
TokenFactory& TokenFactory::manyBefore(const Checker& charsetHas, optional<Error> &&ifNot) {
    return matchingTemplate(
        [charsetHas](const char c){ return !charsetHas(c); },
        [this, charsetHas] {
            while (!charsetHas( lexer->reader.readChar() )) lexer->reader.move();
        },
        true,
        std::move(ifNot)
    );
}

TokenFactory & TokenFactory::forEachReaden(const Checker &isFit, std::optional<Error> ifNot) {
    if (isFailed) return *this;
    for (int i = t.position->index; i < lexer->reader.position().index; i++) {
        if (!isFit( lexer->reader.readCharAt(lexer->reader.position().index - i) )) {
            if (ifNot.has_value())
                regError(std::move(*ifNot));
            isFailed = true;
            break;
        };
    };
    return *this;
}

TokenFactory& TokenFactory::check(const function<bool()> &is) {
    isFailed = is();
    return *this;
}

TokenFactory& TokenFactory::ifFailed(const std::function<void()>& proc) {
    if (isFailed) proc();
    return *this;
}

TokenFactory & TokenFactory::errorIfFailed(const std::string &msg) {
    if (isFailed && t.error == nullptr) regError(makeError(msg));
    return *this;
}

TokenFactory & TokenFactory::pushError(Error &&e) {
    regError(std::move(e));
    return *this;
}

TokenFactory & TokenFactory::ignoreFail() {
    isFailed = false;
    return *this;
};

TokenFactory& TokenFactory::type(const TokenType type) {
    t.type = type;
    return *this;
}

TokenFactory& TokenFactory::matchingTemplate
(const Checker &charsetHas, const Proc &ifSuccessful,
 const bool canBeFailed, optional<Error> &&ifNot)
{
    if (isFailed) return *this;

    if (charsetHas( lexer->reader.readChar() ))
        ifSuccessful();
    else {
        if (ifNot.has_value()) regError(std::move(*ifNot));
        if (canBeFailed) isFailed = true;
    };


    return *this;
};

TokenFactory& TokenFactory::lexeme() {
    t.lexeme = lexer->reader.substrFrom(*t.position);
    return *this;
};

Token TokenFactory::token() {
    t.position = lexer->currentPosition;
    if (!t.lexeme) lexeme();
    if (!t.type  ) t.type = TokenType::Unknown;
    if ( t.error ) t.error->length = getLength();
    auto opt = t;
    clear();
    return Token(*opt.lexeme, *opt.type, opt.position->compact(), opt.error);
}

int TokenFactory::getLength() const { return lexer->reader.position().index - t.position->index; };

void TokenFactory::regError(Error &&err) {
    auto ptr = lexer->errors.put<Error>(std::move(err));
    lexer->errorsList.emplace(ptr);
    if (!t.error) t.error = ptr;
};

Error TokenFactory::makeError(string msg) const {
    return Error {
        *t.position,
        lexer->reader.position().index - t.position->index,
        std::move(msg)
    };
};