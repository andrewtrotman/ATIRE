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
#ifdef FILENAME_INDEX
	char *ANT_search_engine_result_iterator::first(ANT_search_engine *engine, long long start)
#else
	long long ANT_search_engine_result_iterator::first(ANT_search_engine *engine, long long start)
#endif
{
search_engine = engine;
result = engine->results_list;
results_list_length = result->results_list_length;
current = start - 1;

return next();
}

/*
	ANT_SEARCH_ENGINE_RESULT_ITERATOR::NEXT()
	-----------------------------------------
*/
#ifdef FILENAME_INDEX
	char *ANT_search_engine_result_iterator::next(void)
	{
	current++;
	if (current >= results_list_length)
		return NULL;

	return search_engine->get_document_filename(filename_buffer, result->accumulator_pointers[current] - result->accumulator);
	}
#else
	long long ANT_search_engine_result_iterator::next(void)
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
#endif