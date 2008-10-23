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
