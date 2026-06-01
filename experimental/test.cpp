#include <algorithm>
#include <array>
#include <ranges>

template <class From, class To, std::size_t N>
To convertByTable(std::array<const std::pair<From, To>, N> table, From originalValue) {
    return std::ranges::find(
        table,
        originalValue,
        [](const auto &p) { return p.first; }
    )->second;
};

int main() {
    return convertByTable<char, char32_t, 9>({
        std::pair{'n', 0x000A},
        std::pair{'r', 0x000D},
        std::pair{'t', 0x0009},
        std::pair{'0', 0x0000},
        std::pair{'b', 0x0008},
        std::pair{'e', 0x001B},
        std::pair{'"', '"'   },
        std::pair{'\'','\''  },
        std::pair{'\\','\\'  }
    }, 'n');
};