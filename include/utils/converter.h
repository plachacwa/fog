#pragma once
#include <algorithm>
#include <array>
#include <optional>

template <class From, class To, std::size_t N>
std::optional<To> convertByTable(std::array<std::pair<From, To>, N> table, From originalValue) {
    const auto it = std::ranges::find(
        table,
        originalValue,
        [](auto &p) { return p.first; }
    );
    if (it != std::end(table))
        return it->second;
    return std::nullopt;
};