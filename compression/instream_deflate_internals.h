/*
	INSTREAM_DEFLATE_INTERNALS.H
	----------------------------
*/
#ifndef INSTREAM_DEFLATE_INTERNALS_H_
#define INSTREAM_DEFLATE_INTERNALS_H_

#include "../source/memory.h"

/*
	class ANT_INSTREAM_DEFLATE_INTERNALS
	------------------------------------
*/
class ANT_instream_deflate_internals
{
public:
	z_stream stream;

public:
	void *operator new (size_t count, ANT_memory *memory) { return memory->malloc(count); }
} ;

#endif /* INSTREAM_DEFLATE_INTERNALS_H_ */
