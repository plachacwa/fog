#include "common/token.h"
#include <string_view>
using namespace std;

string_view Token::type_str() const {
    switch (type) {
        case TokenType::Integer:        return "Integer";
        case TokenType::Float:          return "Float";
        case TokenType::PrefixedInt:    return "PrefixedInt";
        case TokenType::Exponential:    return "Exponential";
        case TokenType::Symbol:         return "Symbol";
        case TokenType::Char:           return "Char";
        case TokenType::String:         return "String";
        case TokenType::Operator:       return "Operator";
        case TokenType::Punct:          return "Punctuation";
        case TokenType::End:            return "End";
        default:                        return "Unknown";
    };
};