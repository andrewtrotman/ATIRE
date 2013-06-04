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
#include "search_engine_init_flags.h"
#include "search_engine_init_flags_boolean.h"
#include "heap.h"
#include "pregen.h"

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

	ANT_search_engine_accumulator *pregen_scores;
	double pregen_ratio;
	void set_pregen(ANT_pregen *pg, double ratio);
	ANT_ACCUMULATOR_T get_smallest_diff_amoung_the_top()
	{
	long long i;
	ANT_ACCUMULATOR_T smallest, diff;

	qsort(accumulator_pointers, results_list_length, sizeof(*accumulator_pointers), ANT_search_engine_accumulator::cmp_rsv);

	smallest = accumulator_pointers[results_list_length - 2]->get_rsv();
	for (i = results_list_length - 2; i > 0; i--)
		{
		diff = accumulator_pointers[i]->get_rsv() - accumulator_pointers[i - 1]->get_rsv();
		if (smallest > diff)
			smallest = diff;
		}
	return smallest;
	}

	ANT_ACCUMULATOR_T get_diff_between_largest_and_second_largest()
	{
	long long i;
	ANT_search_engine_accumulator *largest, *second_largest;

	// linear scan to find the largest and the second largest
	// because it is a min-heap, large values are at the end of the array,
	// so scan the array in the reverse order
	largest = accumulator_pointers[results_list_length - 2];
	second_largest = accumulator_pointers[results_list_length - 3];
	if (second_largest->get_rsv() > largest->get_rsv())
		{
		largest = second_largest;
		second_largest = accumulator_pointers[results_list_length - 2];
		}
	for (i = results_list_length - 2 - 2; i >=0; i--)
		{
			if (accumulator_pointers[i]->get_rsv() > second_largest->get_rsv())
				{
				if (accumulator_pointers[i]->get_rsv() > largest->get_rsv())
					{
					second_largest = largest;
					largest = accumulator_pointers[i];
					}
				else
					{
					second_largest = accumulator_pointers[i];
					}
				}
		}
	return (largest->get_rsv() - second_largest->get_rsv());
	}

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
	long long width;
	long long width_in_bits;
#endif

protected:
#ifdef TWO_D_ACCUMULATORS
	inline size_t get_init_flag_row(long long index)
	{
	#ifdef TWO_D_ACCUMULATORS_POW2_WIDTH
		return (size_t)(index >> width_in_bits);
	#else
		return (size_t)(index / width);
	#endif
	}
#endif

public:
	ANT_search_engine_result(ANT_memory *memory, long long documents);
	virtual ~ANT_search_engine_result();

#pragma ANT_PRAGMA_NO_DELETE
	void *operator new(size_t bytes, ANT_memory *allocator);

	inline long get_diff_k_and_k_plus_1()
	{
		return (long)(heapk->get_second_smallest()->get_rsv() - accumulator_pointers[0]->get_rsv());
	}

	inline int heap_is_full(void) { return results_list_length >= top_k; }

#ifdef TWO_D_ACCUMULATORS
	/*
		SET_ACCUMULATOR_WIDTH()
		-----------------------
		Note that it is is not necessary to resize the init_flags or the padding in the accumulators
		array because they are already set to the worst possible case.
	*/
	void set_accumulator_width(long long new_width)
	{
	long long height;

	if (new_width == 0)
		new_width = 1;

	if ((width = new_width) == 1)
		height = documents;
	else
		height = (documents / width) + 1;
	init_flags.resize(height);

	init_flags.rewind();
	}
#endif

	inline void init_partial_accumulators(long long index)
	{
#ifdef TWO_D_ACCUMULATORS
	size_t row;
	long long start_acc;
	long long this_width = width;

	if (init_flags.get(row = get_init_flag_row(index)) == 0)
		{
		init_flags.set(row);
		start_acc = row * width;

		// don't write past the end for the last row
		if (start_acc + width > documents)
			this_width = documents - start_acc;

		memcpy(accumulator + start_acc, pregen_scores + start_acc, (size_t)(this_width * sizeof(*accumulator)));
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
	ANT_search_engine_accumulator::ANT_accumulator_t old_value;
	ANT_search_engine_accumulator::compare cmp;

	init_partial_accumulators(index);

	if (results_list_length < top_k)
		{
		/*
			We haven't got enough to worry about the heap yet, so just plonk it in
		*/
		old_value = which->get_rsv();
		which->set_rsv((ANT_search_engine_accumulator::ANT_accumulator_t)score  * pregen_ratio);

		if (old_value == pregen_scores[index].get_rsv())
			accumulator_pointers[results_list_length++] = which;
		if (results_list_length == top_k)
			heapk->build_min_heap();
		}
	else if (cmp(which, accumulator_pointers[0]) >= 0)
		{
		/*
			We were already in the heap, so update
		*/
		which->set_rsv((ANT_search_engine_accumulator::ANT_accumulator_t)score  * pregen_ratio);
		heapk->min_update(which);
		}
	else
		{
		/*
			We weren't in the heap, but we could get put there
		*/
		which->set_rsv((ANT_search_engine_accumulator::ANT_accumulator_t)score * pregen_ratio);
		if (cmp(which, accumulator_pointers[0]) > 0)
			heapk->min_insert(which);
		}
	}

	template <class T> inline void add_rsv(long long index, T score)
	{
	ANT_search_engine_accumulator *which = accumulator + index;
	ANT_search_engine_accumulator::ANT_accumulator_t old_value;
	ANT_search_engine_accumulator::compare cmp;

	init_partial_accumulators(index);

	if (results_list_length < top_k)
		{
		/*
			We haven't got enough to worry about the heap yet, so just plonk it in
		*/
		old_value = which->get_rsv();
		which->add_rsv(score * pregen_ratio);

		if (old_value == pregen_scores[index].get_rsv())
			accumulator_pointers[results_list_length++] = which;
		if (results_list_length == top_k)
			heapk->build_min_heap();
		}
	else if (cmp(which, accumulator_pointers[0]) >= 0)
		{
		/*
			We were already in the heap, so update
		*/
		which->add_rsv(score * pregen_ratio);
		heapk->min_update(which);
		}
	else
		{
		/*
			We weren't in the heap, but we could get put there
		*/
		which->add_rsv(score * pregen_ratio);
		if (cmp(which, accumulator_pointers[0]) > 0)
			heapk->min_insert(which);
		}
	}

	long is_zero_rsv(long long index)
		{
#ifdef TWO_D_ACCUMULATORS
		if (init_flags.get(get_init_flag_row(index)) == 0)
			return true;
	else
		{
		//return accumulator[(size_t)index].is_zero_rsv();
		return accumulator[(size_t)index].get_rsv() == pregen_scores[(size_t)index].get_rsv();
		}
#else
		//return accumulator[(size_t)index].is_zero_rsv();
		return accumulator[(size_t)index].get_rsv() == pregen_scores[(size_t)index].get_rsv();
#endif
		}

	void init_accumulators(long long top_k);
	long long init_pointers(void);
} ;


#endif /* SEARCH_ENGINE_RESULT_H_ */
