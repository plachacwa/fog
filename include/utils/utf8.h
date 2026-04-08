#pragma once
#include <utility>

std::pair<char32_t, int> decodeUtf8At(const char*, std::size_t);
