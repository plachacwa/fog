#include "enums.h"
#include <unordered_map>
using namespace std;
using Token = Type::TokenT;

static unordered_map<Token, string_view>
tokentitles = {
	{Token::Symbol, 	"Symbol"},
	{Token::Integer,	"Integer"},
	{Token::Float,		"Float"},
	{Token::Char,		"Char"},
	{Token::String,		"String"},
	{Token::Operator,	"Operator"},
	{Token::Keyword,	"Keyword"},
	{Token::Punct,		"Punct"},
	{Token::End,		"End"},
	{Token::Unknown,	"Unknown"}
};

std::string_view Type::get(Token val) {
	auto it = tokentitles.find(val);
	return it->second;
};
