/*
	INSTREAM_BZ2_INTERNALS.H
	------------------------
*/
#ifndef INSTREAM_BZ2_INTERNALS_H_
#define INSTREAM_BZ2_INTERNALS_H_

#include "memory.h"

/*
	class ANT_INSTREAM_BZ2_INTERNALS
	--------------------------------
*/
class ANT_instream_bz2_internals
{
public:
#ifdef ANT_HAS_BZLIB
	bz_stream stream;
#endif

public:
	void *operator new (size_t count, ANT_memory *memory) { return memory->malloc(count); }
} ;

#endif /* INSTREAM_BZ2_INTERNALS_H_ */
