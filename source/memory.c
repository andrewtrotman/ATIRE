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
ANT_memory::ANT_memory(long long block_size_for_allocation)
{
chunk_end = at = chunk = NULL;
used = 0;
this->block_size = block_size_for_allocation;
}

/*
	ANT_MEMORY::~ANT_MEMORY()
	-------------------------
*/
ANT_memory::~ANT_memory()
{
delete [] chunk;
}
