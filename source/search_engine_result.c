/*
	SEARCH_ENGINE_RESULT.C
	----------------------
*/
#include "search_engine_result.h"
#include "memory.h"
#include "maths.h"
#include "search_engine_accumulator.h"

/*
	ANT_SEARCH_ENGINE_RESULT::ANT_SEARCH_ENGINE_RESULT()
	----------------------------------------------------
*/
ANT_search_engine_result::ANT_search_engine_result(ANT_memory *memory, long long documents)
{
long long pointer;
unsigned long long padding = 0;
#ifdef TWO_D_ACCUMULATORS
long long height = 0;
#endif

/* initialise the pregen scores */
pregen_scores = new ANT_search_engine_accumulator[documents];
memset(pregen_scores, 0, sizeof(*pregen_scores)*documents);
pregen_ratio = 1.0;

results_list_length = 0;
min_in_top_k = 0;

this->documents = documents;

#ifdef TWO_D_ACCUMULATORS
	width_in_bits = 8;
	width = ANT_pow2_zero_64(width_in_bits);
	height = (documents / width) + 1;
	/*
		The worst case is that each accumulator is its own row in which case height is equal to documents
		in which case we need to allocate documents-number of init_flags

		The worst case for the padding is when there a N documents in the collection and the width is set
		to N-1.  In this case there are N-1 wasted accumulators  A good estimate of this is N itself.
	*/
	init_flags.init(documents);
	init_flags.set_length(height);
	padding = documents;
#endif

memory->realign();
accumulator = (ANT_search_engine_accumulator *)memory->malloc(sizeof(*accumulator) * (documents + padding));
memory->realign();
accumulator_pointers = (ANT_search_engine_accumulator **)memory->malloc(sizeof(*accumulator_pointers) * documents);
for (pointer = 0; pointer < documents; pointer++)
	accumulator_pointers[pointer] = &accumulator[pointer];

top_k = 10;			// this is given a true value later and the heap is then resized - but in the mean time give it a default of 10

heapk = new ANT_heap<ANT_search_engine_accumulator *, ANT_search_engine_accumulator::compare>(*accumulator_pointers, top_k);
}

/*
	ANT_SEARCH_ENGINE_RESULT::~ANT_SEARCH_ENGINE_RESULT()
	-----------------------------------------------------
*/
ANT_search_engine_result::~ANT_search_engine_result()
{
delete heapk;
delete [] pregen_scores;
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
//	init_flags.rewind();		// this is now done later based on a computed width
#else
	memcpy(accumulator, pregen_scores, sizeof(*accumulator) * documents);
#endif

min_in_top_k = 0;
this->top_k = top_k;
results_list_length = 0;
heapk->set_size(top_k);
}

/*
	ANT_SEARCH_ENGINE_RESULT::INIT_POINTERS()
	-----------------------------------------
*/
long long ANT_search_engine_result::init_pointers(void)
{
return results_list_length;
}

/*
	ANT_SEARCH_ENGINE_RESULT::SET_PREGEN()
	-----------------------------------------
*/
void ANT_search_engine_result::set_pregen(ANT_pregen *pg, double ratio)
{
if (pg)
	{
	pregen_ratio = ratio;
	for (long long i = 0; i < documents; i++)
		{
		pregen_scores[i].add_rsv(log((double)pg->scores[i]) * (1 - pregen_ratio) - 0.01);
		//pregen_scores[i].add_rsv(log((double)pg->scores[i]) * 0.1 - 0.01);

		//printf("orig: %lld, scores: %f\n", pg->scores[i], pregen_scores[i].get_rsv());
		//pregen_scores[i].set_rsv((ANT_ACCUMULATOR_T)pg->scores[i]);
		}
	}
}
