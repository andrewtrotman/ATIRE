/*
 * MAX_QUANTUM_POINTER
 */

#ifndef MAX_QUANTUM_POINTER
#define MAX_QUANTUM_POINTER

class ANT_impact_header;
class ANT_search_engine_btree_leaf;

class ANT_max_quantum {
public:
	ANT_impact_header *the_impact_header;
	long long current_max_quantum;
	long long quantum_count;
	ANT_search_engine_btree_leaf *term_details;

	struct compare
	{
		int operator() (ANT_max_quantum *a, ANT_max_quantum *b) const
		{
			return a->current_max_quantum - b->current_max_quantum;
		}
	};
};

#endif /* MAX_QUANTUM_POINTER */
