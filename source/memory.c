/*
	MEMORY.C
	--------
*/
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
/*
	ANT_MEMORY::ANT_MEMORY()
	------------------------
*/
ANT_memory::ANT_memory()
{
chunk_end = at = chunk = NULL;
used = 0;
}

/*
	ANT_MEMORY::~ANT_MEMORY()
	-------------------------
*/
ANT_memory::~ANT_memory()
{
delete [] chunk;
}

/*
	ANT_MEMORY::MALLOC()
	--------------------
*/
void *ANT_memory::malloc(size_t bytes)
{
void *ans;
#pragma omp critical
	{
	if (chunk == NULL)
		{
		allocated = 1024 * 1024 * 1024;
		at = chunk = new char [(size_t)allocated];
		chunk_end = chunk + allocated;
		}
	ans = at;
	at += bytes;
	if (at > chunk_end)
		exit(printf("Out of memory:%d bytes requested %d bytes used", bytes, used));
	used += bytes;
	}
return ans;
}
