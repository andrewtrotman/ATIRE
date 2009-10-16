/*
	SEARCH_ENGINE_ACCUMULATOR_ARRAY.C
	---------------------------------
*/
#include <string.h>
#include "search_engine_accumulator_array.h"
#include "memory.h"

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR_ARRAY::ANT_SEARCH_ENGINE_ACCUMULATOR_ARRAY()
	--------------------------------------------------------------------------
*/
ANT_search_engine_accumulator_array::ANT_search_engine_accumulator_array(ANT_memory *memory, long long documents)
{
long long pointer;

this->documents = documents;
memory->realign();
accumulator = (ANT_search_engine_accumulator *)memory->malloc(sizeof(*accumulator) * documents);
memory->realign();
accumulator_pointers = (ANT_search_engine_accumulator **)memory->malloc(sizeof(*accumulator_pointers) * documents);
for (pointer = 0; pointer < documents; pointer++)
	accumulator_pointers[pointer] = &accumulator[pointer];
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR_ARRARY::OPERATOR NEW()
	----------------------------------------------------
*/
void *ANT_search_engine_accumulator_array::operator new(size_t count, ANT_memory *allocator)
{
return allocator->malloc(count);
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR_ARRAY::INIT_ACCUMULATORS()
	--------------------------------------------------------
*/
void ANT_search_engine_accumulator_array::init_accumulators(void)
{
memset(accumulator, 0, (size_t)(sizeof(*accumulator) * documents));
}

/*
	ANT_SEARCH_ENGINE_ACCUMULATOR_ARRAY::INIT_POINTERS()
	----------------------------------------------------
*/
long long ANT_search_engine_accumulator_array::init_pointers(void)
{
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
	the first particion of the quick-sort before the call to quick sort.  The advantage
	is that we know in advance what the correct partition value is and that the secone
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
}
