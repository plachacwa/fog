#include <memory>

TEST(expr_digit) {
    auto tokens = Lexer("42").tokenize();
	GlobalContext ctx;
	auto node = dynamic_cast<Atom*>(Parser(tokens, ctx).parse().get());
	assert(node != nullptr);
	auto literal = static_cast<Literal*>(node->info_);
    assert(literal->type->name == "int");
	assert(literal->value == "42");
}