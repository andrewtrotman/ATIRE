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
	ANT_POSTINGS_PIECE::ANT_POSTINGS_PIECE()
	----------------------------------------
*/
ANT_postings_piece::ANT_postings_piece(ANT_memory *allocator, long bytes_wanted)
{
data = (unsigned char *)allocator->malloc(bytes_wanted);
next = NULL;
}

