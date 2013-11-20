/*
	MAX_QUANTUM.H
	-------------
*/
#ifndef MAX_QUANTUM_H_
#define MAX_QUANTUM_H_

class ANT_impact_header;
class ANT_search_engine_btree_leaf;

/*
	class ANT_MAX_QUANTUM
	---------------------
*/
class ANT_max_quantum
{
public:
	ANT_impact_header *the_impact_header;
	long long current_max_quantum;
	long long quantum_count;
	ANT_search_engine_btree_leaf *term_details;

	struct compare
		{
		long long operator() (ANT_max_quantum *a, ANT_max_quantum *b) const { return a->current_max_quantum - b->current_max_quantum; }
		};
};


#endif /* MAX_QUANTUM_H_ */
