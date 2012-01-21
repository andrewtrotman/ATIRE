/*
	COMPRESS_TEXT_DEFLATE_INTERNALS.H
	---------------------------------
*/
#ifndef COMPRESS_TEXT_DEFLATE_INTERNALS_H_
#define COMPRESS_TEXT_DEFLATE_INTERNALS_H_

#ifdef ANT_HAS_ZLIB
	#include "zlib.h"
#endif

/*
	class ANT_COMPRESS_TEXT_DEFLATE_INTERNALS
	-----------------------------------------
*/
class ANT_compress_text_deflate_internals
{
#ifdef ANT_HAS_ZLIB
	public:
		z_stream stream;		// for compression (deflation)
		z_stream instream;		// for decompression (inflation)
#endif
};

#endif /* COMPRESS_TEXT_DEFLATE_INTERNALS_H_ */
