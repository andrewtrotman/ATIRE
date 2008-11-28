/*
	POSTINGS_PIECE.H
	----------------
*/

#ifndef __POSTINGS_PIECE_H__
#define __POSTINGS_PIECE_H__

class ANT_memory;

class ANT_postings_piece
{
public:
	unsigned char *data;
	ANT_postings_piece *next;

public:
	ANT_postings_piece(ANT_memory *alloc, long required_length);
	void *operator new (size_t size, ANT_memory *allocator);
} ;

#endif /* __POSTINGS_PIECE_H__ */

