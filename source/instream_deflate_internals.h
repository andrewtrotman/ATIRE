/*
	INSTREAM_DEFLATE_INTERNALS.H
	----------------------------
*/
#ifndef INSTREAM_DEFLATE_INTERNALS_H_
#define INSTREAM_DEFLATE_INTERNALS_H_

#include "pragma.h"
#include "memory.h"

/*
	class ANT_INSTREAM_DEFLATE_INTERNALS
	------------------------------------
*/
class ANT_instream_deflate_internals
{
public:
#ifdef ANT_HAS_ZLIB
	z_stream stream;
#endif

public:
#pragma ANT_PRAGMA_NO_DELETE
	void *operator new (size_t count, ANT_memory *memory) { return memory->malloc(count); }
} ;

#endif /* INSTREAM_DEFLATE_INTERNALS_H_ */
