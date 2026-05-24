#include "parser/optable.h"
using namespace std;
using namespace Builtin;

OpTable::OpTable() {
	operatorTable.reserve(basicOperatorsCount);
	priorityTable.reserve(basicOperators.size());
	
	for ( const auto &priorityVec : basicOperators ) {
		vector<const Op*> priorityLevel{};
		priorityLevel.reserve( priorityVec.size() );
		
		for ( const auto &op : priorityVec ) {
			const auto *ptr = &op;
			operatorTable.push_back( std::pair{ op.lexeme, ptr } );
			priorityLevel.push_back( ptr );
		};
		priorityTable.push_back( move(priorityLevel) );
	};
}