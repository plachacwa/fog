#include <optional>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
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
		cout << left << Type::get(elem.type) << ": \t"
		<< setw(20) << elem.lexeme << " ["
		<< elem.pos.line << ":"
		<< elem.pos.column << "]\n";
	};
};