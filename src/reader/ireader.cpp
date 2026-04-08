#include <fstream>
#include <ranges>

#include "reader/ireader.h"
using namespace std;

std::optional<std::string> readFile(const std::string &path) {
	std::ifstream file(path);
	if (!file.is_open())
		return std::nullopt;

	auto range = std::ranges::subrange(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	return std::string(range.begin(), range.end());
};