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

	void add_to_top_k(size_t index)
		{
		size_t index_of_smallest;
		ANT_search_engine_accumulator::ANT_accumulator_t value_of_smallest;

		if (results_list_length < top_k)
			accumulator_pointers[results_list_length++] = accumulator + index;		// just put us into the array
		else
			{
			size_t current;
			/*
				If we sort here then we're O(n log n) but if we pass the array twice then we're O(2n)
			*/
			index_of_smallest = 0;
			value_of_smallest = accumulator_pointers[0]->get_rsv();
			/*
				Find the smallest value in the top-k
			*/
			for (current = 1; current < top_k; current++)
				if (accumulator_pointers[current]->get_rsv() < value_of_smallest)
					{
					index_of_smallest = current;
					value_of_smallest = accumulator_pointers[current]->get_rsv();
					}
			/*
				swap that for the new accumulator
			*/
			accumulator_pointers[index_of_smallest] = accumulator + index;

			/*
				now find the smallest accumulator
			*/
			min_in_top_k = accumulator_pointers[0]->get_rsv();
			for (current = 1; current < top_k; current++)
				if (accumulator_pointers[current]->get_rsv() < min_in_top_k)
					min_in_top_k = accumulator_pointers[current]->get_rsv();
			}
		}

	void add_rsv(size_t index, long score)
		{
		if (accumulator[index].get_rsv() > min_in_top_k)
			accumulator[index].add_rsv(score);				// we're already in the top-k so just add.
		else
			{
			if (accumulator[index].add_rsv(score) > min_in_top_k)
				add_to_top_k(index);
			}
		}
	void add_rsv(size_t index, double score)
		{
		if (accumulator[index].get_rsv() > min_in_top_k)
			accumulator[index].add_rsv(score);				// we're already in the top-k so just add.
		else
			{
			if (accumulator[index].add_rsv(score) > min_in_top_k)
				add_to_top_k(index);
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
