#pragma once
#include <functional>
#include <optional>

#include "charsets.h"
#include "common/token.h"
#include "common/error.h"

class Lexer;
class StandardReader;

class TokenFactory {
    Lexer       *lexer;
    public: bool isFailed = false;

    struct {
        std::optional<std::string_view> lexeme;
        std::optional<TokenType> type;
        std::optional<BigPosition> position;
        Error *error = nullptr;
    } t;

    private:
        using Checker = std::function<bool(Codepoint)>;
        using Proc    = std::function<void()>;

    public:
        explicit TokenFactory(Lexer*);
        TokenFactory& clear();

        TokenFactory& one       (const Checker& = Charset::AnyExceptNull,
                                 std::optional<Error>&& = std::nullopt);
        TokenFactory& many      (const Checker&, std::optional<Error>&& = std::nullopt);
        TokenFactory& maybeOne  (const Checker& = Charset::AnyExceptNull);
        TokenFactory& maybeMany (const Checker&);
        TokenFactory& manyBefore(const Checker&, std::optional<Error>&& = std::nullopt);

        TokenFactory& forEachReaden(const Checker&, std::optional<Error> = std::nullopt);
        TokenFactory& check        (const std::function<bool()> &);

        TokenFactory& ifFailed(const std::function<void()>&);
        TokenFactory& errorIfFailed(const std::string&);
        TokenFactory& pushError(Error&&);
        TokenFactory& ignoreFail();

        TokenFactory& type(TokenType);
        TokenFactory& lexeme();

        [[nodiscard]] Token token();

        [[nodiscard]] Error makeError(std::string msg) const;
    private:
        [[nodiscard]] int getLength() const;
        void regError(Error&&);

        TokenFactory& matchingTemplate
            (const Checker&, const Proc&,
             bool, std::optional<Error>&& = std::nullopt);
};