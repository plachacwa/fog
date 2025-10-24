#include <optional>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "token.h"
using namespace std;

optional<string> readFile(const char* filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		return nullopt;
	};
	
	ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
};

void printTokens(vector<Token> tokens) {
	for (auto &elem : tokens) {
		cout << Type::get(elem.type) <<
			": " << elem.lexeme << '\n';
	};
};