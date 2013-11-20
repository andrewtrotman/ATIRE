/*
	POSTINGS_PIECE.H
	----------------
*/

#ifndef POSTINGS_PIECE_H_
#define POSTINGS_PIECE_H_

#include "pragma.h"

class ANT_memory;

/*
	class ANT_POSTINGS_PIECE
	------------------------
*/
class ANT_postings_piece
{
public:
	unsigned short *data;
	ANT_postings_piece *next;

public:
	ANT_postings_piece() {}
	unsigned short *initialise(ANT_memory *alloc, size_t required_length);
#pragma ANT_PRAGMA_NO_DELETE
	void *operator new (size_t size, ANT_memory *allocator);
} ;

#endif  /* POSTINGS_PIECE_H_ */

