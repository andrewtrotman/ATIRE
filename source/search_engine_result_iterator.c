/*
	SEARCH_ENGINE_RESULT_ITERATOR.C
	-------------------------------
*/
#include "search_engine.h"
#include "search_engine_result.h"
#include "search_engine_result_iterator.h"

/*
	ANT_SEARCH_ENGINE_RESULT_ITERATOR::FIRST()
	------------------------------------------
*/
long long ANT_search_engine_result_iterator::first(ANT_search_engine *engine, long long start)
{
search_engine = engine;
result = engine->results_list;
#ifdef TOP_K_SEARCH
	results_list_length = result->results_list_length;
#else
	results_list_length = engine->document_count();
#endif
current = start - 1;
return next();
}

/*
	ANT_SEARCH_ENGINE_RESULT_ITERATOR::NEXT()
	-----------------------------------------
*/
long long ANT_search_engine_result_iterator::next(void)
{
#ifdef NEVER
	/*
		This is the old "rsv can be negative" version
	*/
	current++;
	while (current < results_list_length)
		if (result->accumulator_pointers[current]->is_zero_rsv())
			current++;
		else
			return result->accumulator_pointers[current] - result->accumulator;

	return -1;

#else
	/*
		New-fangled rvs must be positive version
	*/
	current++;
	if (current > results_list_length)
		return -1;

	if (result->accumulator_pointers[current]->is_zero_rsv())
		return -1;
	else
		return result->accumulator_pointers[current] - result->accumulator;
#endif
}