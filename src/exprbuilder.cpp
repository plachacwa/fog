#include "exprbuilder.h"
#include "utils.h"
#include "parser.h"
#include <unordered_map>
#include <cassert>
using namespace std;

TypeInfo* ExprBuilder::getTypeInfo(Token::Type toktype) const {
	string_view name;
	static const unordered_map<Token::Type, string_view> table {
		{ Token::Symbol, 			"sym" 	},
		{ Token::Integer,			"int" 	},
		{ Token::Float,				"float" },
		{ Token::Char,				"char"	},
		{ Token::String,			"str" 	},
		{ Token::KeywordFuncLike, 	"sym" 	},
		{ Token::KeywordValue,	 	"sym" 	}
    };
	auto it = table.find(toktype);
	if (it != table.end())
		return pos.scope->typePtr(name);
	else
		return nullptr;
};

ExprBuilder::ExprBuilder(Parser *p)
	: nodes(), operators(), parser(p), pos(p->pos) {};
	
/*  
 *	Функция parse() должна пройти всё выражение, добравшись до т.н. конца выражения,
 *	и вернуть единственный получившийся узел. Частью выражения считается любое значение,
 *	включая любые идентификаторы и ключевые слова-идентификаторы, оператор (не являющийся
 *	ключевым словом), а также знаки пунктуации, за исключением точки с запятой
 *	и фигурных скобок (обрабатываются основным парсером отдельно).
 */
uNode ExprBuilder::parse() {
	while (!pos.isEnd() && parseToken());
	if (nodes.size() != 1)
		assert(false && "ExprBuilder::parse(): Output vector size is not 1");
	return move(nodes.back());
};

/*
 *	parseToken() читает текущий токен и либо добавляет его в список нод или операторов,
 *	либо прекращает обработку выражения. Функция может прочитать следующий токен и определить,
 *	требуется ли закончить обработку выражения (т.е. выполнение функции parse()). Если
 *	обрабатываемый токен является завершающим, то функция возвращает false, если нет -
 *	то true.
 */
bool ExprBuilder::parseToken() {
	const auto &t = pos.read();
	bool isEndOfExpr = true;
	
	switch (t.type) {
		case Token::Symbol:
		case Token::Integer:
		case Token::Float:
		case Token::Char:
		case Token::String:
		case Token::KeywordFuncLike:
		case Token::KeywordValue:
			isEndOfExpr = parseAtom(t);
			pos.fwd();
			break;
		case Token::Operator:
			isEndOfExpr = parseOperator(t);
			pos.fwd();
			break;
		case Token::KeywordStmt:
			//nodes.push_back(parser.parseInstruction());
			return false;
		case Token::End:
			return false;
		default:
			parseAtom(t);
			pos.fwd();
			break;
	};
	
	return isEndOfExpr;
};

bool ExprBuilder::parseAtom(const Token& t) {
	const auto typeinfo = getTypeInfo(t.type);
	if (!typeinfo)
		throw runtime_error("Unknown type");
	const auto literal = pos.ctx->addLiteral(t.lexeme, typeinfo);
	auto node = make_unique<Atom>(literal, typeinfo);
	nodes.push_back(move(node));
	return true;
};

bool ExprBuilder::parseOperator(const Token& t) {
	operators.push_back(t);
	return true;
};