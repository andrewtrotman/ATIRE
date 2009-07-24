/*
	SEARCH_ENGINE_ACCUMULATOR.H
	---------------------------
*/

#ifndef __SEARCH_ENGINE_ACCUMULATOR_H__
#define __SEARCH_ENGINE_ACCUMULATOR_H__

#include <stddef.h>		// needed for size_t

class ANT_search_engine_accumulator
{
public:
	/*
		ANT_accumulator_t is the type used for storing rsv scores in the search engine.  Experiments with 50 topics
		on the TREC WSJ collection suggest that using fixed point arrithmatic accurate to 2 decimal places is faster than
		using floating point arrithmatic.  The effect on precision is in the 5th decimal place (negligable) and so can be ignored.
		For higher accuracy at an execution cost set ANT_accumulator_t to a double (or float).
	*/
	typedef short ANT_accumulator_t;			// short or double

private:
	ANT_accumulator_t rsv;

private:
	static inline void swap(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b) { register ANT_search_engine_accumulator *tmp; tmp = *a; *a = *b; *b = tmp; } 
	static void top_k_shortsort(ANT_search_engine_accumulator **lo, ANT_search_engine_accumulator **hi);
	static inline int compare_pointer(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b);

public:
	void add_rsv(double score) { rsv += (ANT_accumulator_t)(100 * score); }
	void add_rsv(long score) { rsv += (ANT_accumulator_t)score; }
	long is_zero_rsv(void) { return rsv == 0; }

	static int compare(const void *a, const void *b);
	static int compare_pointer(const void *a, const void *b);
	static inline int compare_pointer(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator *b);
	static void top_k_sort(ANT_search_engine_accumulator **base, long long num, long long top_k);

	static inline long long min(long long a, long long b);
	static inline ANT_search_engine_accumulator **med3(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, ANT_search_engine_accumulator **c);
	static inline void swapfunc(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b, long long n);
} ;

#endif __SEARCH_ENGINE_ACCUMULATOR_H__
