/*
	SEARCH_ENGINE_RESULT.C
	----------------------
*/
#include "search_engine_result.h"
#include "memory.h"
#include "maths.h"
#include "pdebug.h"

/*
	ANT_SEARCH_ENGINE_RESULT::ANT_SEARCH_ENGINE_RESULT()
	----------------------------------------------------
*/
ANT_search_engine_result::ANT_search_engine_result(ANT_memory *memory, long long documents)
{
long long pointer;
unsigned long long padding = 0;

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
results_list_length = min_in_top_k = 0;
#endif

this->documents = documents;

#ifdef TWO_D_ACCUMULATORS
	width_in_bits = 8;
	width = ANT_pow2_zero(width_in_bits);
	height = (documents / width) + 1;
	memory->realign();
	init_flags = (unsigned char *)memory->malloc(sizeof(*init_flags) * height);
	memset(init_flags, 0, sizeof(*init_flags) * height);
	padding = (width * height) - documents;
	dbg_printf("padding: %llu\n", (padding));
#endif

memory->realign();
accumulator = (ANT_search_engine_accumulator *)memory->malloc(sizeof(*accumulator) * (documents+padding));
memory->realign();
accumulator_pointers = (ANT_search_engine_accumulator **)memory->malloc(sizeof(*accumulator_pointers) * documents);
for (pointer = 0; pointer < documents; pointer++)
	accumulator_pointers[pointer] = &accumulator[pointer];

#ifdef HEAP_K_SEARCH
	heapk = new Heap<ANT_search_engine_accumulator *, ANT_search_engine_accumulator_cmp>(*accumulator_pointers, top_k);
	include_set = new ANT_bitstring();
	include_set->set_length(documents);
#endif
}

/*
	ANT_SEARCH_ENGINE_RESULT::~ANT_SEARCH_ENGINE_RESULT()
	-----------------------------------------------------
*/
ANT_search_engine_result::~ANT_search_engine_result()
{
#ifdef HEAP_K_SEARCH
	delete heapk;
#endif
}

/*
	ANT_SEARCH_ENGINE_RESULT::OPERATOR NEW()
	----------------------------------------
*/
void *ANT_search_engine_result::operator new(size_t count, ANT_memory *allocator)
{
return allocator->malloc(count);
}

/*
	ANT_SEARCH_ENGINE_RESULT::INIT_ACCUMULATORS()
	---------------------------------------------
*/
#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
void ANT_search_engine_result::init_accumulators(long long top_k)
#else
void ANT_search_engine_result::init_accumulators(void)
#endif
{
#ifdef TWO_D_ACCUMULATORS
memset(init_flags, 0, sizeof(*init_flags) * height);
#else
memset(accumulator, 0, (size_t)(sizeof(*accumulator) * documents));
#endif

#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	min_in_top_k = 0;
	this->top_k = top_k;
	results_list_length = 0;
	#ifdef HEAP_K_SEARCH
		heapk->set_size(top_k);
		include_set->zero();
	#endif
#endif
}

/*
	ANT_SEARCH_ENGINE_RESULT::INIT_POINTERS()
	-----------------------------------------
*/
long long ANT_search_engine_result::init_pointers(void)
{
#if (defined TOP_K_SEARCH) || (defined HEAP_K_SEARCH)
	return results_list_length;
#else

#ifdef TWO_D_ACCUMULATORS
	unsigned long long current_accumulator, end_accumulator;
	ANT_search_engine_accumulator **forward_pointer, **backward_pointer;

	forward_pointer = accumulator_pointers;
	backward_pointer = accumulator_pointers + documents - 1;

	for (unsigned long long row = 0; row < height; row++) {
		if (init_flags[row] == 1) {
			end_accumulator = ((row+1) * width) > documents ? documents : (row + 1) * width;
			for (current_accumulator = row * width; current_accumulator < end_accumulator; current_accumulator++) {
				if (accumulator[current_accumulator].is_zero_rsv()) {
					*backward_pointer-- = &(accumulator[current_accumulator]);
				} else {
					*forward_pointer++ = &(accumulator[current_accumulator]);
				}
			}
		}
	}

	return forward_pointer - accumulator_pointers;

#else

	ANT_search_engine_accumulator **current, **back_current, *current_accumulator, *end_accumulator;

	/*
		On first observations it appears as though this array does not need to be
		re-initialised because the accumulator_pointers array already has a pointer
		to each accumulator, but they are left in a random order from the previous
		sort - which is good news (right?). Actually, all the zeros are left at the
		end which leads to a pathological case in quick-sort taking tens of seconds
		on the INEX Wikipedia 2009 collection.

		An effective optimisation is to bucket sort into two buckets at the beginning,
		one bucket is the zeros and the other bucket is the non-zeros.  This is essentially
		the first partition of the quick-sort before the call to quick-sort.  The advantage
		is that we know in advance what the correct partition value is and that the second
		partition (of all zeros) is now already sorted.  We also get (for free) the number
		of documents we found.
	*/
	current = accumulator_pointers;
	back_current = accumulator_pointers + documents - 1;
	end_accumulator = accumulator + documents;
	for (current_accumulator = accumulator; current_accumulator < end_accumulator; current_accumulator++)
		if (current_accumulator->is_zero_rsv())
			*back_current-- = current_accumulator;
		else
			*current++ = current_accumulator;

	/*
		Return the number of relevant documents.
	*/
	return current - accumulator_pointers;
#endif

#endif
}
