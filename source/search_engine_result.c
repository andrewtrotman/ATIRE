/*
	SEARCH_ENGINE_RESULT.C
	----------------------
*/
#include "search_engine_result.h"
#include "memory.h"
#include "maths.h"

/*
	ANT_SEARCH_ENGINE_RESULT::ANT_SEARCH_ENGINE_RESULT()
	----------------------------------------------------
*/
ANT_search_engine_result::ANT_search_engine_result(ANT_memory *memory, long long documents)
{
long long pointer;
unsigned long long padding = 0;

results_list_length = 0;
min_in_top_k = 0;

this->documents = documents;

#ifdef TWO_D_ACCUMULATORS
	width_in_bits = 8;
	width = ANT_pow2_zero_64(width_in_bits);
	height = (documents / width) + 1;
	memory->realign();
	init_flags = (unsigned char *)memory->malloc(sizeof(*init_flags) * height);
	memset(init_flags, 0, (size_t)(sizeof(*init_flags) * height));
	padding = (width * height) - documents;
	//printf("padding: %llu\n", (padding));
#endif

memory->realign();
accumulator = (ANT_search_engine_accumulator *)memory->malloc(sizeof(*accumulator) * (documents + padding));
memory->realign();
accumulator_pointers = (ANT_search_engine_accumulator **)memory->malloc(sizeof(*accumulator_pointers) * documents);
for (pointer = 0; pointer < documents; pointer++)
	accumulator_pointers[pointer] = &accumulator[pointer];

top_k = 10;			// this is given a true value later and the heap is then resized - but in the mean time give it a default of 10
heapk = new Heap<ANT_search_engine_accumulator *, ANT_search_engine_accumulator::compare>(*accumulator_pointers, top_k);
include_set = new ANT_bitstring();
include_set->set_length(documents);
}

/*
	ANT_SEARCH_ENGINE_RESULT::~ANT_SEARCH_ENGINE_RESULT()
	-----------------------------------------------------
*/
ANT_search_engine_result::~ANT_search_engine_result()
{
delete heapk;
delete include_set;
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
void ANT_search_engine_result::init_accumulators(long long top_k)
{
#ifdef TWO_D_ACCUMULATORS
memset(init_flags, 0, (size_t)(sizeof(*init_flags) * height));
#else
memset(accumulator, 0, (size_t)(sizeof(*accumulator) * documents));
#endif

min_in_top_k = 0;
this->top_k = top_k;
results_list_length = 0;
heapk->set_size(top_k);
include_set->zero();
}

/*
	ANT_SEARCH_ENGINE_RESULT::INIT_POINTERS()
	-----------------------------------------
*/
long long ANT_search_engine_result::init_pointers(void)
{
return results_list_length;
}
