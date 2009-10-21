/*
	SEARCH_ENGINE_RESULT.H
	----------------------
*/
#ifndef SEARCH_ENGINE_RESULT_H_
#define SEARCH_ENGINE_RESULT_H_


#include <stdio.h>
#include "search_engine_accumulator.h"

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
public:		// REMOVE THIS LINE
	ANT_search_engine_accumulator *accumulator;
	ANT_search_engine_accumulator **accumulator_pointers;
	long long documents;

public:
	ANT_search_engine_result(ANT_memory *memory, long long documents);
	virtual ~ANT_search_engine_result() {}

	void *operator new(size_t bytes, ANT_memory *allocator);
	ANT_search_engine_accumulator &operator[](size_t index) { return accumulator[index]; }

#ifdef TOP_K_SEARCH
	long long top_k;
	long long results_list_length;
	ANT_search_engine_accumulator::ANT_accumulator_t min_in_top_k;

	void add_to_top_k(ANT_search_engine_accumulator *which_accumulator, long was_zero)
		{
		size_t index_of_smallest, current;
		ANT_search_engine_accumulator::ANT_accumulator_t value_of_smallest;

		if (results_list_length < top_k)
			{
			/*
				If we were previously a zero-valued RSV then we cannot be in the top-k accumulators
				list and so we add it.  If we were a non-zero-valued rsv then we're already there
			*/
			if (was_zero)
				accumulator_pointers[results_list_length++] = which_accumulator;
			}
		else
			{
			/*
				If we sort here then we're O(n log n) but if we pass the array twice then we're O(2n)
			*/
			index_of_smallest = 0;
			value_of_smallest = accumulator_pointers[0]->get_rsv();
			/*
				Find the smallest value in the top-k
			*/
			for (current = 0; current < top_k; current++)
				{
				if (accumulator_pointers[current] == which_accumulator)
					{
					/*
						This is the "nasty" case where the score is equal to min_in_top_k
						and the given element is in the top-k (it might not be if, for exmaple
						k+1 documents have the minimum score required to be in the top-k).

						By setting index_of_smallest to current the result is that the pointer
						is updated to itself and the new min in computed (which is necessary
						because there might be only one document with a score of min_in_top_k.
					*/
					index_of_smallest = current;
					break;
					}
				if (accumulator_pointers[current]->get_rsv() < value_of_smallest)
					{
					index_of_smallest = current;
					value_of_smallest = accumulator_pointers[current]->get_rsv();
					}
				}
			/*
				swap that for the new accumulator
			*/
			accumulator_pointers[index_of_smallest] = which_accumulator;

			/*
				now find the now-smallest accumulator
			*/
			min_in_top_k = accumulator_pointers[0]->get_rsv();
			for (current = 1; current < top_k; current++)
				if (accumulator_pointers[current]->get_rsv() < min_in_top_k)
					min_in_top_k = accumulator_pointers[current]->get_rsv();
			}
		}
	template <class T> inline void add_rsv(size_t index, T score)
		{
		long was_zero;
		ANT_search_engine_accumulator *which = accumulator + index; 

		if (which->get_rsv() > min_in_top_k)
			which->add_rsv(score);				// we're already in the top-k so just add.
		else
			{
			was_zero = which->is_zero_rsv();
			if (which->add_rsv(score) > min_in_top_k)
 				add_to_top_k(which, was_zero);
			}
		}
#else
	void add_rsv(size_t index, double score) { accumulator[index].add_rsv(score); }
	void add_rsv(size_t index, long score) { accumulator[index].add_rsv(score); }
#endif

	long is_zero_rsv(size_t index) { return accumulator[index].is_zero_rsv(); }
	
	void init_accumulators(void);
	long long init_pointers(void);
} ;


#endif /* SEARCH_ENGINE_RESULT_H_ */
