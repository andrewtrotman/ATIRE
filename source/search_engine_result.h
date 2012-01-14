/*
	SEARCH_ENGINE_RESULT.H
	----------------------
*/
#ifndef SEARCH_ENGINE_RESULT_H_
#define SEARCH_ENGINE_RESULT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pragma.h"
#include "search_engine_accumulator.h"
#include "bitstring.h"
#include "search_engine_init_flags.h"
#include "search_engine_init_flags_boolean.h"
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
	#ifdef NEVER
		/*
			In this case we use unsigned chars for each flag.  Its fast to check a flag but slow to initialise the flags.
		*/
		ANT_search_engine_init_flags init_flags;			// using unsigned chars
	#else
		/*
			In this case we use a bit for each flag.  Its slow to check a flag but fast to initialise the flags.
		*/
		ANT_search_engine_init_flags_boolean init_flags;	// using a bitstring
	#endif
	long long width, height;
	long long width_in_bits;
#endif

protected:
#ifdef TWO_D_ACCUMULATORS
	inline size_t get_init_flag_row(long long index)
	{
	#ifdef TWO_D_ACCUMULATORS_VARIABLE_WIDTH
		return (size_t)(index / width);
	#else
		return (size_t)(index >> width_in_bits);
	#endif
	}
#endif

public:
	ANT_search_engine_result(ANT_memory *memory, long long documents);
	virtual ~ANT_search_engine_result();

#pragma ANT_PRAGMA_NO_DELETE
	void *operator new(size_t bytes, ANT_memory *allocator);

#ifdef TWO_D_ACCUMULATORS
	/*
		SET_ACCUMULATOR_WIDTH()
		-----------------------
		Note that it is is not necessary to resize the init_flags or the padding in the accumulators
		array because they are already set to the worst possible case.
	*/
	void set_accumulator_width(long long new_width) { width = new_width; }
#endif

	inline void init_partial_accumulators(long long index)
	{
#ifdef TWO_D_ACCUMULATORS
	size_t row;

	if (init_flags.get(row = get_init_flag_row(index)) == 0)
		{
		init_flags.set(row);
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
		if (init_flags.get(get_init_flag_row(index)) == 0)
			return true;
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
