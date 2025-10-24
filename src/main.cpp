#ifndef NDEBUG
    #define ENABLE_TESTS
#endif
#ifdef ENABLE_TESTS
	#include "tests.h"
#endif

#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include "lexer.h"
using namespace std;

#ifdef ENABLE_TESTS
	#include "tests/tests.inl"
#endif

optional<string> readFile(const char* filename);
void printTokens(vector<Token> tokens);

int main() {
	optional code = readFile("bin/test.fg");
	vector<Token> tokens = Lexer(*code).tokenize();
	printTokens(tokens);
	return 0;
};