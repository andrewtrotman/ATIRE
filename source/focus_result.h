/*
	FOCUS_RESULT.H
	--------------
*/
#ifndef FOCUS_RESULT_H_
#define FOCUS_RESULT_H_

#include "search_engine_accumulator.h"

/*
	class ANT_FOCUS_RESULT
	----------------------
*/
class ANT_focus_result
{
friend class ANT_focus_result_factory;

public:
	char *start, *finish;
	long long INEX_start, INEX_finish;
	ANT_search_engine_accumulator::ANT_accumulator_t rsv;		// use what ever type we are using in the search engine

	ANT_search_engine_accumulator::ANT_accumulator_t add_rsv(double score) { return rsv += 1 + (ANT_search_engine_accumulator::ANT_accumulator_t)(score * 100.0); }		// +1 to prevent rounding to zero
	ANT_search_engine_accumulator::ANT_accumulator_t add_rsv(long score) { return rsv += (ANT_search_engine_accumulator::ANT_accumulator_t)score; }

	ANT_search_engine_accumulator::ANT_accumulator_t get_rsv(void) { return rsv; }
	void clear_rsv(void) { rsv = 0; }
} ;

#endif /* FOCUS_RESULT_H_ */
