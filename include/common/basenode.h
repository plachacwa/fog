#pragma once
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include "parser/operator.h"

struct Type {};

using Literal = std::variant<
    int,
    double,
    char32_t,
    std::string_view
>;

struct SymbolInfo {};

struct ASTNode {
    using InfoVariants =
    std::variant<std::monostate, Literal, SymbolInfo*, const Op*>;

    std::monostate        type{};
    InfoVariants          info{};
    std::vector<ASTNode*> children;

    public:
        explicit ASTNode() = default;

        explicit ASTNode(InfoVariants &&info)
        : info(info) {};

        explicit ASTNode(std::vector<ASTNode*> children)
        : children(std::move(children)) {};

        explicit ASTNode(InfoVariants &&info, std::vector<ASTNode*> children)
        : info(info), children(std::move(children)) {};

        [[nodiscard]] virtual const char* name() const = 0;

        void print(int indent = 0) const;

        virtual ~ASTNode() = default;
        
        template <class T>
        [[nodiscard]] const T* thisIs() const {
            return dynamic_cast<const T*>(this);
        };

        [[nodiscard]] virtual std::string infoStr() const { return ""; };
};