TEST(lexer_digit) {
    auto tokens = Lexer("42").tokenize();
    vector samples = {
        Token{Token::Integer, "42"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_float) {
    auto tokens = Lexer("3.14").tokenize();
    vector samples = {
        Token{Token::Float, "3.14"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_symbol) {
    auto tokens = Lexer("foo").tokenize();
    vector samples = {
        Token{Token::Symbol, "foo"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_keyword) {
    auto tokens = Lexer("interface").tokenize();
    vector samples = {
        Token{Token::Keyword, "interface"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_parens) {
    auto tokens = Lexer("()").tokenize();
    vector samples = {
        Token{Token::Punct, "("},
        Token{Token::Punct, ")"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
    assert(tokens[2].is(samples[2]));
}

TEST(lexer_braces) {
    auto tokens = Lexer("{}").tokenize();
    vector samples = {
        Token{Token::Punct, "{"},
        Token{Token::Punct, "}"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
    assert(tokens[2].is(samples[2]));
}

TEST(lexer_semicolon) {
    auto tokens = Lexer(";").tokenize();
    vector samples = {
        Token{Token::Punct, ";"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_operator) {
    auto tokens = Lexer("+ - * /").tokenize();
    vector samples = {
        Token{Token::Operator, "+"},
        Token{Token::Operator, "-"},
        Token{Token::Operator, "*"},
        Token{Token::Operator, "/"},
        Token{Token::End, ""},
    };
    for (size_t i = 0; i < tokens.size(); ++i)
        assert(tokens[i].is(samples[i]));
}

TEST(lexer_comment_single_line) {
    auto tokens = Lexer("-- this is a comment\n42").tokenize();
    vector samples = {
        Token{Token::Integer, "42"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_comment_block) {
    auto tokens = Lexer("{- block comment -}42").tokenize();
    vector samples = {
        Token{Token::Integer, "42"},
        Token{Token::End, ""},
    };
    assert(tokens[0].is(samples[0]));
    assert(tokens[1].is(samples[1]));
}

TEST(lexer_empty) {
    auto tokens = Lexer("").tokenize();
    std::vector<Token> samples = {
        Token{Token::End, ""}
    };
    assert(tokens.size() == 1);
    assert(tokens[0].is(samples[0]));
}

TEST(lexer_mixed) {
    auto tokens = Lexer("class main() { x = 42; }").tokenize();
    vector samples = {
        Token{Token::Keyword, "class"},
        Token{Token::Symbol, "main"},
        Token{Token::Punct, "("},
        Token{Token::Punct, ")"},
        Token{Token::Punct, "{"},
        Token{Token::Symbol, "x"},
        Token{Token::Operator, "="},
        Token{Token::Integer, "42"},
        Token{Token::Punct, ";"},
        Token{Token::Punct, "}"},
        Token{Token::End, ""},
    };
    for (size_t i = 0; i < tokens.size(); ++i)
        assert(tokens[i].is(samples[i]));
}