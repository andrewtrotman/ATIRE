/*
	SEARCH_ENGINE_RESULT_ID_ITERATOR.C
	----------------------------------
*/
#include "search_engine.h"
#include "search_engine_result.h"
#include "search_engine_result_id_iterator.h"

/*
	ANT_SEARCH_ENGINE_RESULT_ID_ITERATOR::FIRST()
	---------------------------------------------
*/
long long ANT_search_engine_result_id_iterator::first(ANT_search_engine_result *results_list, long long start)
{
result = results_list;
results_list_length = result->results_list_length;
current = start - 1;

return next();
}

/*
	ANT_SEARCH_ENGINE_RESULT_ID_ITERATOR::NEXT()
	--------------------------------------------
*/
long long ANT_search_engine_result_id_iterator::next(void)
{
/*
	New-fangled rvs must be positive version
*/
current++;
if (current >= results_list_length)
	return -1;

if (result->is_zero_rsv(result->accumulator_pointers[current] - result->accumulator))
	return -1;
else
	return result->accumulator_pointers[current] - result->accumulator;
}
