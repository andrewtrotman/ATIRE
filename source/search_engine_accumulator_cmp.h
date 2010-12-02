/*
	SEARCH_ENGINE_ACCUMULATOR_CMP.H
	-------------------------------
*/
#ifndef SEARCH_ENGINE_ACCUMULATOR_CMP_H_
#define SEARCH_ENGINE_ACCUMULATOR_CMP_H_

/*
	class ANT_SEARCH_ENGINE_ACCUMULATOR_CMP
	---------------------------------------
*/
class ANT_search_engine_accumulator_cmp
{
public:
	inline int operator() (ANT_search_engine_accumulator *a, ANT_search_engine_accumulator *b) { return a->get_rsv() < b->get_rsv() ? -1 : a->get_rsv() == b->get_rsv() ? 0 : 1; }
} ;

#endif /* SEARCH_ENGINE_ACCUMULATOR_CMP_H_ */
