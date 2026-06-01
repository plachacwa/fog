#pragma once
#include <algorithm>
#include <expected>
#include <functional>
#include <optional>
#include <plf_hive.h>
#include <string_view>
#include <variant>

#include "builtin.h"
#include "operator.h"
#include "utils/enumerate.h"

class OpTable {
    plf::hive<Op>                                                     extendedOperators;
    std::vector<std::pair<const char*, const Op*>>                    operatorTable;
    std::vector<std::pair<Op::Associativity, std::vector<const Op*>>> priorityTable;

    public:
        OpTable();

        OpTable(OpTable &&) noexcept = default;

        OpTable& operator=(OpTable &&) noexcept = default;

        OpTable(const OpTable &) = delete;

        OpTable& operator=(const OpTable &) = delete;

        std::expected<std::monostate, std::string_view>
        push(const Op *sample, int offsetFromSample, Op::Associativity associativity, Op &&op);
        std::expected<std::monostate, std::string_view>
        push(const Op *sample, Op&&);

        const Op* find(std::string_view soughtForLexeme, Op::Arity soughtForArity);

        [[nodiscard]] std::size_t limit() const;

        std::optional<int> getOpPriority(const Op *op);

        std::optional<Op::Associativity> getOpAssociativity(const Op *op);

    private:
        template <class T>
        std::optional<T> findingOpInfoTemplate(const Op *sample, std::function<T(int)> fn);
};

template <class T>
std::optional<T> OpTable::findingOpInfoTemplate(const Op *sample, std::function<T(int)> fn) {
    for (auto [idx, vec]: enumerate(priorityTable)) {
        if (auto it = std::ranges::find(vec.second, sample);
            it == vec.second.end())
            continue;
        return fn(idx);
    };
    return std::nullopt;
};
