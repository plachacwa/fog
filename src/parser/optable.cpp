#include "parser/optable.h"
using namespace std;
using namespace Builtin;

OpTable::OpTable() {
    operatorTable.reserve(basicOperatorsCount);
    priorityTable.reserve(basicOperators.size());

    for (const auto &[associativity, opVec]: basicOperators) {
        vector<const Op*> priorityLevel{};
        priorityLevel.reserve(opVec.size());

        for (const auto &op: opVec) {
            const auto *ptr = &op;
            operatorTable.emplace_back(op.lexeme, ptr);
            priorityLevel.push_back(ptr);
        };
        priorityTable.emplace_back(associativity, std::move(priorityLevel));
    };
};

const Op* OpTable::find(const string_view soughtForLexeme, const Op::Arity soughtForArity) {
    for (auto [lexeme, op]: operatorTable)
        if (string_view(lexeme) == soughtForLexeme && op->arity == soughtForArity)
            return op;
    return nullptr;
};
size_t OpTable::limit() const { return priorityTable.size(); };

optional<int> OpTable::getOpPriority(const Op *op) {
    return findingOpInfoTemplate<int>(op, [](const int idx) { return idx; });
};

optional<Op::Associativity> OpTable::getOpAssociativity(const Op *op) {
    return findingOpInfoTemplate<Op::Associativity>(
        op,
        [&](const int idx) { return priorityTable[idx].first; }
    );
};

expected<monostate, string_view> OpTable::push(
    const Op *sample, const int offsetFromSample, Op::Associativity associativity, Op &&op
) {
    if (find(op.lexeme, op.arity) != nullptr)
        return unexpected("attempt to add existing operator");
    if (op.arity == Binary && associativity == EqAssociative)
        return unexpected("binary operator cannot have equal associativity");
    if (op.arity == Multiple && associativity != NonAssociative)
        associativity = EqAssociative;

    const auto newOp = &*extendedOperators.insert(op);
    operatorTable.emplace_back(newOp->lexeme, newOp);

    const auto result = findingOpInfoTemplate<optional<string_view>>(
        sample, [&](const int idx) -> optional<string_view> {
            if (offsetFromSample == 0) {
                auto &[sampleAssociativity, sampleVec] = priorityTable[idx + offsetFromSample];
                if (sampleAssociativity != associativity)
                    return "the associativity of the new op does not match that of the sample";
                sampleVec.emplace_back(newOp);
            } else
                priorityTable.insert(
                    priorityTable.begin() + (idx + offsetFromSample),
                    { associativity, { newOp } }
                );
            return nullopt;
        }
    );
    if (!result.has_value())
        return unexpected("the sample op is not in this optable");
    if (result.has_value() && result->has_value())
        return unexpected(**result);
    return monostate();
};

expected<monostate, string_view> OpTable::push(const Op *sample, Op &&op) {
    return push(sample, 0, getOpAssociativity(sample).value(), std::move(op));
};
