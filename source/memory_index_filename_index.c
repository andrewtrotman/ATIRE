/*
	MEMORY_INDEX_FILENAME_INDEX.H
	-----------------------------
*/
#ifndef MEMORY_INDEX_FILENAME_INDEX_H_
#define MEMORY_INDEX_FILENAME_INDEX_H_

#include <stdio.h>
#include <stdlib.h>

/*
	class ANT_MEMORY_INDEX_FILENAME_INDEX
	-------------------------------------
*/
class ANT_memory_index_filename_index
{
protected:
//	static const size_t members_per_chunk = 4 * 1024 * 1024;
//	static const size_t chunk_growth_rate = 10;
	static const size_t members_per_chunk = 3;
	static const size_t chunk_growth_rate = 2;

protected:
	size_t chunks_allocated;
	size_t chunks_used;
	size_t members_used;
	long long **chunk;

public:
	ANT_memory_index_filename_index();
	~ANT_memory_index_filename_index();
	void add(long long value);

	void text_render(void);
} ;

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
	ANT_MEMORY_INDEX_FILENAME_INDEX::TEXT_RENDER()
	----------------------------------------------
*/
void ANT_memory_index_filename_index::text_render(void)
{
size_t current, member;

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

/*
	MAIN()
	------
*/
int main(void)
{
ANT_memory_index_filename_index array;
long long number;

for (number = 0; number < 1; number++)
	array.add(number);

array.text_render();
}


#endif