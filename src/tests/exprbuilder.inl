#include <memory>

TEST(expr_digit) {
    auto tokens = Lexer("42").tokenize();
	auto node = dynamic_cast<Atom*>(Parser(tokens).parse().get());
    auto sample = make_unique<Atom>("42", "int");
    assert(node->value_ == sample->value_);
	assert(node->type_.type == sample->type_.type);
}