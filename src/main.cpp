#include <iostream>
#include <string>
#include <optional>
using namespace std;

optional<string> readFile(const char* filename);

int main() {
	optional code = readFile("bin/test.fg");
	cout << *code << '\n';
	return 0;
};