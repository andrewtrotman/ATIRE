/*
	SEARCH_ENGINE_ACCUMULATORS.H
	----------------------------
*/

#ifndef __SEARCH_ENGINE_ACCUMULATORS_H__
#define __SEARCH_ENGINE_ACCUMULATORS_H__

#include <stdlib.h>

class ANT_search_engine_accumulator
{
private:
	static inline void swap(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b) { register ANT_search_engine_accumulator *tmp; tmp = *a; *a = *b; *b = tmp; } 
	static void top_k_shortsort(ANT_search_engine_accumulator **lo, ANT_search_engine_accumulator **hi);
	static inline int compare_pointer(ANT_search_engine_accumulator **a, ANT_search_engine_accumulator **b);

public:

//	double rsv;
	float rsv;

public:
	static int compare(const void *a, const void *b);
	static int compare_pointer(const void *a, const void *b);
	static void top_k_sort(ANT_search_engine_accumulator **base, size_t num, long top_k);
} ;

#endif __SEARCH_ENGINE_ACCUMULATORS_H__
