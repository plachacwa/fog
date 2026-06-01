#include "common/token.h"
#include "utils/interval.h"

#include <cassert>
#include <string_view>
using namespace std;

string_view Token::type_str() const {
    switch (type) {
        case TokenType::Integer:        return "Integer";
        case TokenType::Float:          return "Float";
        case TokenType::PrefixedInt:    return "PrefixedInt";
        case TokenType::Exponential:    return "Exponential";
        case TokenType::Keyword:        return "Keyword";
        case TokenType::WeakKeyword:    return "WeakKeyword";
        case TokenType::Symbol:         return "Symbol";
        case TokenType::PossibleFlag:   return "PossibleFlag";
        case TokenType::Directive:      return "Directive";
        case TokenType::Char:           return "Char";
        case TokenType::String:         return "String";
        case TokenType::Operator:       return "Operator";
        case TokenType::Colon:          return "Colon";
        case TokenType::Semicolon:      return "Semicolon";
        case TokenType::End:            return "End";
        case TokenType::Unknown:        return "Unknown";
        default:
            if (type <= Interval(TokenType::LCB, TokenType::RRB))
                return "Bracket";
            else
                assert(false);
    };
};