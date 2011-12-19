/*
	SEARCH_ENGINE_RESULT.H
	----------------------
*/
#ifndef SEARCH_ENGINE_RESULT_H_
#define SEARCH_ENGINE_RESULT_H_

#include <stdio.h>
#include "pragma.h"
#include "search_engine_accumulator.h"
#include "bitstring.h"
#include <string.h>

#include "heap.h"

class ANT_memory;

/*
	class ANT_SEARCH_ENGINE_RESULT
	------------------------------
*/
class ANT_search_engine_result
{
friend class ANT_search_engine;
friend class ANT_search_engine_result_iterator;

private:
public:			// remove this line later
	ANT_search_engine_accumulator *accumulator;
	ANT_search_engine_accumulator **accumulator_pointers;
	long long documents;
	long long results_list_length;

	long long top_k;
	ANT_search_engine_accumulator::ANT_accumulator_t min_in_top_k;

	ANT_heap<ANT_search_engine_accumulator *, ANT_search_engine_accumulator::compare> *heapk;
	ANT_bitstring *include_set;

#ifdef TWO_D_ACCUMULATORS
	unsigned char *init_flags;
	long long width, height;
	long long width_in_bits;
#endif

public:
	ANT_search_engine_result(ANT_memory *memory, long long documents);
	virtual ~ANT_search_engine_result();

#pragma ANT_PRAGMA_NO_DELETE
	void *operator new(size_t bytes, ANT_memory *allocator);

	inline void init_partial_accumulators(long long index)
	{
#ifdef TWO_D_ACCUMULATORS
	unsigned long long row;

	if (init_flags[row = (index >> width_in_bits)] == 0)
		{
		init_flags[row] = 1;
		memset(accumulator + (row * width), 0, (size_t)(width * sizeof(*accumulator)));
		}
#endif
	}

	inline ANT_search_engine_accumulator &operator[](size_t index)
	{
	init_partial_accumulators(index);
	return accumulator[index];
	}


	template <class T> inline void set_rsv(long long index, T score)
	{
	ANT_search_engine_accumulator *which = accumulator + index;
	ANT_search_engine_accumulator::ANT_accumulator_t old_val;
	ANT_search_engine_accumulator::compare cmp;

	init_partial_accumulators(index);
	old_val = which->get_rsv();
	which->set_rsv((ANT_search_engine_accumulator::ANT_accumulator_t) score);

	if (results_list_length < top_k)
		{
		if (old_val == 0)
			{
			accumulator_pointers[results_list_length++] = which;
			include_set->unsafe_setbit(index);
			}
		if (results_list_length == top_k)
			heapk->build_min_heap();
		}
	else if (include_set->unsafe_getbit(index))
			heapk->min_update(which);
	else if (cmp(which, accumulator_pointers[0]) > 0)
		{
		include_set->unsafe_unsetbit((long)(accumulator_pointers[0] - accumulator));
		heapk->min_insert(which);
		include_set->unsafe_setbit(index);
		}
	}

	template <class T> inline void add_rsv(long long index, T score)
	{
	ANT_search_engine_accumulator *which = accumulator + index;
	ANT_search_engine_accumulator::ANT_accumulator_t old_val;
	ANT_search_engine_accumulator::ANT_accumulator_t new_val;
	ANT_search_engine_accumulator::compare cmp;

	init_partial_accumulators(index);
	old_val = which->get_rsv();
	new_val = which->add_rsv(score);

	if (results_list_length < top_k)
		{
		if (old_val == 0)
			{
			accumulator_pointers[results_list_length++] = which;
			include_set->unsafe_setbit(index);
			}
		if (results_list_length == top_k)
			heapk->build_min_heap();
		}
	else if (include_set->unsafe_getbit(index))
		heapk->min_update(which);
	else if (cmp(which, accumulator_pointers[0]) > 0)
		{
		include_set->unsafe_unsetbit((long)(accumulator_pointers[0] - accumulator));
		heapk->min_insert(which);
		include_set->unsafe_setbit(index);
		}
	}

	long is_zero_rsv(long long index)
		{
#ifdef TWO_D_ACCUMULATORS
		if (init_flags[index >> width_in_bits] == 0)
			return 0 == 0;
		else
			return accumulator[(size_t)index].is_zero_rsv();
#else
		return accumulator[(size_t)index].is_zero_rsv();
#endif
		}

	void init_accumulators(long long top_k);
	long long init_pointers(void);
} ;


#endif /* SEARCH_ENGINE_RESULT_H_ */
