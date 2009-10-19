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
long long ANT_search_engine_result_iterator::first(ANT_search_engine *engine)
{
search_engine = engine;
result = engine->results_list;
results_list_length = engine->document_count();
current = -1;
return next();
}

/*
	ANT_SEARCH_ENGINE_RESULT_ITERATOR::NEXT()
	-----------------------------------------
*/
long long ANT_search_engine_result_iterator::next(void)
{
current++;
while (current < results_list_length)
	if (result->accumulator_pointers[current]->is_zero_rsv())
		current++;
	else
		return result->accumulator_pointers[current] - result->accumulator;
return -1;
}