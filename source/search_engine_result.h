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

#ifdef HEAP_K_SEARCH
#include "heap.h"
#endif

#include "pdebug.h"


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

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	long long top_k;
	long long results_list_length;
	ANT_search_engine_accumulator::ANT_accumulator_t min_in_top_k;
#endif

#ifdef HEAP_K_SEARCH
private:
	struct cmp_accumulator_pointers {
		int operator() (ANT_search_engine_accumulator * a, ANT_search_engine_accumulator * b) {
			return a->get_rsv() < b->get_rsv() ? -1 : a->get_rsv() == b->get_rsv() ? 0 : 1;
		}
	};
	Heap<ANT_search_engine_accumulator *, cmp_accumulator_pointers> *heapk;
	ANT_bitstring *include_set;
#endif

#ifdef TWO_D_ACCUMULATORS
	unsigned char *init_flags;
	unsigned long width, height;
	unsigned short width_in_bits;
#endif

public:
	ANT_search_engine_result(ANT_memory *memory, long long documents);
	virtual ~ANT_search_engine_result();

#pragma ANT_PRAGMA_NO_DELETE
	void *operator new(size_t bytes, ANT_memory *allocator);
	ANT_search_engine_accumulator &operator[](size_t index) { return accumulator[index]; }

	inline void init_partial_accumulators(size_t index) {
#ifdef TWO_D_ACCUMULATORS
		unsigned long long row;
		if (init_flags[row = (index >> width_in_bits)] == 0) {
			init_flags[row] = 1;
			memset(accumulator + (row * width), 0, width * sizeof(*accumulator));
		}
#endif
	}

#ifdef TOP_K_SEARCH

	void reshuffle_top_k(ANT_search_engine_accumulator *which_accumulator)
		{
		size_t index_of_smallest, current;
		ANT_search_engine_accumulator::ANT_accumulator_t value_of_smallest;

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

	template <class T> inline void add_rsv(long index, T score)
		{
		ANT_search_engine_accumulator::ANT_accumulator_t was;
		ANT_search_engine_accumulator *which = accumulator + index;

		init_partial_accumulators(index);

//		if (index > documents)
//			printf("docid %d too big\n", index);

		if ((was = which->get_rsv()) <= min_in_top_k)
			{									// were' not in the top-k but we might end up there.
			if (which->add_rsv(score) > min_in_top_k)
				if (results_list_length < top_k)
					{							// the top-k is not full so add only if we're not alrady there
					if (was == 0)
						accumulator_pointers[results_list_length++] = which;
					}
				else
	 				reshuffle_top_k(which);		// we have to evict someone from the top-k
			}
		else
			which->add_rsv(score);				// we're already in the top-k so just add.
		}
#elif defined HEAP_K_SEARCH

#ifdef ANDREW_HEAP_K
	template <class T> inline void add_rsv(long index, T score) {
		ANT_search_engine_accumulator *which = accumulator + index;
		ANT_search_engine_accumulator::ANT_accumulator_t old_val = which->get_rsv();
		ANT_search_engine_accumulator::ANT_accumulator_t new_val;

		init_partial_accumulators(index);
		new_val = which->add_rsv(score);

		//printf("andrew_k\n");

		if (results_list_length < top_k) {
			if (old_val == 0) {
				accumulator_pointers[results_list_length++] = which;
				include_set->unsafe_setbit(index);
			}
			// build the heap when there are top k number of non-zero accumulators
			if (results_list_length == top_k) {
				heapk->build_min_heap();
				min_in_top_k = accumulator_pointers[0]->get_rsv();
			}
		} else {
			// if the accumulator was previously not in the heap and now the
			// new rsv value is greater than the minimum in the heap, then
			// added this accumulator into the heap by replacing it with the minimum
			if ((old_val <= min_in_top_k) && (new_val > min_in_top_k)) {
				if (!include_set->unsafe_getbit(index)) {
					include_set->unsafe_unsetbit(accumulator_pointers[0] - accumulator);
					accumulator_pointers[0] = which;
					include_set->unsafe_setbit(index);
				}
				heapk->build_min_heap();
				min_in_top_k = accumulator_pointers[0]->get_rsv();
			}
		}
	}
#endif

#ifdef FEI_HEAP_K
	template <class T> inline void add_rsv(long index, T score) {
		ANT_search_engine_accumulator *which = accumulator + index;
		ANT_search_engine_accumulator::ANT_accumulator_t old_val = which->get_rsv();
		ANT_search_engine_accumulator::ANT_accumulator_t new_val;

		init_partial_accumulators(index);
		new_val = which->add_rsv(score);
		//printf("fei_k\n");
		if (results_list_length < top_k) {
			if (old_val == 0) {
				accumulator_pointers[results_list_length++] = which;
			}
		} else {
			if ((old_val <= min_in_top_k) && (new_val >= min_in_top_k)) {
				if (min_in_top_k > accumulator_pointers[0]->get_rsv()) {
					heapk->build_min_heap();
				}
				accumulator_pointers[0] = which;

				heapk->build_min_heap();
				min_in_top_k = accumulator_pointers[0]->get_rsv();
			}
		}
	}
#endif


#else
	void add_rsv(size_t index, double score) {  init_partial_accumulators(index); accumulator[index].add_rsv(score); }
	void add_rsv(size_t index, long score) {  init_partial_accumulators(index); accumulator[index].add_rsv(score); }
#endif

	long is_zero_rsv(size_t index) { return accumulator[index].is_zero_rsv(); }

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	void init_accumulators(long long top_k);
#else
	void init_accumulators(void);
#endif
	long long init_pointers(void);
} ;


#endif /* SEARCH_ENGINE_RESULT_H_ */
