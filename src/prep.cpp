#include <optional>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
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