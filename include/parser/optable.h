#pragma once
#include <unordered_map>
#include <algorithm>
#include <string_view>
#include <optional>

#include "builtin.h"
#include "operator.h"
#include "common/region.h"
#include "utils/enumerate.h"
#include "utils/interval.h"

class OpTable {
    Region extendedOperators;
    std::vector< std::pair<const char*, const Op*> > operatorTable;
    std::vector< std::vector<const Op*> >              priorityTable;

    public:
        OpTable (OpTable &&) noexcept = default;
        OpTable &operator=(OpTable &&) noexcept = default;

        OpTable (const OpTable&) = delete;
        OpTable &operator=(const OpTable&) = delete;

        OpTable ();
        
        template <class ...Args>
        bool push( const Op *sample, int offsetFromSample, Args ...args ) {
            if (find( sample->lexeme, sample->arity ) != nullptr) return false;
            const auto *newOp = extendedOperators.make<Op>( std::forward<Args>(args)... );
            operatorTable.push_back({ newOp->lexeme, newOp });
            
            return findingOpPriorityTemplate<std::monostate>(sample, [](int idx){
                       const auto totalOffset = idx+offsetFromSample;
                       if ( totalOffset <= Interval(0, priorityTable.size() - 1) )
                           priorityTable[ idx+offsetFromSample ].push_back( newOp );
                       else if ( totalOffset < 0 )
                           priorityTable.push_front({ newOp });
                       else
                           priorityTable.push_back ({ newOp });
                   }).has_value();
        };
        
        const Op* find( std::string_view soughtForLexeme, Op::Arity soughtForArity ) {
            for ( auto [ lexeme, op ] : operatorTable )
                if ( std::string_view( lexeme ) == soughtForLexeme && op->arity == soughtForArity )
                    return op;
            return nullptr;
        };
        
        std::size_t limit() const { return priorityTable.size(); };
        
        std::optional<int> getOpPriority(const Op *op) {
            return findingOpPriorityTemplate<int>( op, []( int idx ) { return idx; });
        };
    
    private:
        template <class T>
        std::optional<T> findingOpPriorityTemplate(const Op* sample, std::function<T( int )> fn) {
            for ( auto [ idx, vec ] : enumerate( priorityTable ) ) {
                auto it = std::find( vec.begin(), vec.end(), sample );
                if ( it == vec.end() ) continue;
                return fn( idx );
            };
            return std::nullopt;
        };
};
