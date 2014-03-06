/*
	MEMORY_INDEX_FILENAME_INDEX.C
	-----------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_index_filename_index.h"

/*
	ANT_MEMORY_INDEX_FILENAME_INDEX::ANT_MEMORY_INDEX_FILENAME_INDEX()
	------------------------------------------------------------------
*/
ANT_memory_index_filename_index::ANT_memory_index_filename_index()
{
chunk = NULL;
chunks_allocated = 0;
chunks_used = -1;
members_used = members_per_chunk + 1;
}

/*
	ANT_MEMORY_INDEX_FILENAME_INDEX::~ANT_MEMORY_INDEX_FILENAME_INDEX()
	-------------------------------------------------------------------
*/
ANT_memory_index_filename_index::~ANT_memory_index_filename_index()
{
long current_chunk;

if (chunk != NULL)
	{
	for (current_chunk = 0; current_chunk <= chunks_used; current_chunk++)
		delete [] chunk[current_chunk];

	free(chunk);
	}
}

/*
	ANT_MEMORY_INDEX_FILENAME_INDEX::ADD()
	--------------------------------------
*/
void ANT_memory_index_filename_index::add(long long value)
{
if (members_used >= members_per_chunk)
	{
	chunks_used++;
	if (chunks_used >= chunks_allocated)
		{
		chunks_allocated += chunk_growth_rate;
		chunk = (long long **)realloc(chunk, sizeof(*chunk) * chunks_allocated);
		}
	chunk[chunks_used] = new long long [members_per_chunk];
	members_used = 0;
	}
chunk[chunks_used][members_used] = value;
members_used++;
}

/*
	ANT_MEMORY_INDEX_FILENAME_INDEX::SERIALISE()
	--------------------------------------------
*/
long long *ANT_memory_index_filename_index::serialise(void)
{
long long current, *answer, *into;

if (chunk == NULL)
	return NULL;

into = answer = new long long [(size_t)members()];

for (current = 0; current < chunks_used; current++)
	{
	memcpy(into, chunk[current], sizeof(**chunk) * members_per_chunk);
	into += members_per_chunk;
	}

memcpy(into, chunk[chunks_used], sizeof(**chunk) * members_used);

return answer;
}

/*
	ANT_MEMORY_INDEX_FILENAME_INDEX::TEXT_RENDER()
	----------------------------------------------
*/
void ANT_memory_index_filename_index::text_render(void)
{
long long current, member;

if (chunk == NULL)
	puts("0 elements in array");
else
	{
	printf("%lld elements in array\n", (long long)((chunks_used) * members_per_chunk + members_used));

	for (current = 0; current < chunks_used; current++)
		for (member = 0; member < members_per_chunk; member++)
			printf("%lld\n", chunk[current][member]);

	for (member = 0; member < members_used; member++)
		printf("%lld\n", chunk[chunks_used][member]);
	}
}
