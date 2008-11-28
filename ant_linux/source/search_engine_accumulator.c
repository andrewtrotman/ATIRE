/*
	SEARCH_ENGINE_ACCUMULATOR.C
	---------------------------
*/
#include "search_engine_accumulator.h"

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR::COMPARE()
	----------------------------------------
*/
int ANT_search_engine_accumulator::compare(const void *a, const void *b)
{
double sign;

sign = ((ANT_search_engine_accumulator *)a)->rsv - ((ANT_search_engine_accumulator *)b)->rsv;

return sign < 0.0 ? 1 : sign > 0.0 ? -1 : 0;
}
