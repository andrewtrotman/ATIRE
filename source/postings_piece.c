/*
	POSTINGS_PIECE.C
	----------------
*/
#include <stdio.h>
#include "postings_piece.h"
#include "memory.h"

/*
	ANT_POSTINGS_PIECE::OPERATOR NEW()
	----------------------------------
*/
void *ANT_postings_piece::operator new (size_t bytes, ANT_memory *allocator)
{
return allocator->malloc(bytes);
}

/*
	ANT_POSTINGS_PIECE::INITIALISE()
	--------------------------------
*/
unsigned short *ANT_postings_piece::initialise(ANT_memory *allocator, long bytes_wanted)
{
data = (unsigned short *)allocator->malloc(bytes_wanted);
next = NULL;

return data;
}

