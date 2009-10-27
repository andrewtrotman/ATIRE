/*
	COMPRESS_TEXT_DEFLATE.C
	-----------------------
*/
#include <new>
#include "compress_text_deflate.h"
#include "compress_text_deflate_internals.h"

/*
	ANT_COMPRESS_TEXT_DEFLATE::ANT_COMPRESS_TEXT_DEFLATE()
	------------------------------------------------------
*/
ANT_compress_text_deflate::ANT_compress_text_deflate() : ANT_compress_text()
{
#ifdef ANT_HAS_ZLIB
	const int level = 9;		// maximum compression
	internals = new (std::nothrow) ANT_compress_text_deflate_internals;

	internals->stream.zalloc = Z_NULL;
	internals->stream.zfree = Z_NULL;
	internals->stream.opaque = Z_NULL;

	deflateInit(&internals->stream, level);
#endif
}

/*
	ANT_COMPRESS_TEXT_DEFLATE::~ANT_COMPRESS_TEXT_DEFLATE()
	-------------------------------------------------------
*/
ANT_compress_text_deflate::~ANT_compress_text_deflate()
{
#ifdef ANT_HAS_ZLIB
	deflateEnd(&internals->stream);
#endif
}

/*
	ANT_COMPRESS_TEXT_DEFLATE::COMPRESS()
	-------------------------------------
*/
char *ANT_compress_text_deflate::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_ZLIB
	unsigned long success;

	if (deflateReset(&internals->stream) != Z_OK)
		return NULL;

	internals->stream.avail_in = source_length;
	internals->stream.next_in = (Bytef *)source;
	internals->stream.avail_out = *destination_length;
	internals->stream.next_out = (Bytef *)destination;

	success = deflate(&internals->stream, Z_FINISH);    /* no bad return value */
	*destination_length = *destination_length - internals->stream.avail_out;

	return success == Z_STREAM_END ? destination : NULL;
#else
	return NULL;
#endif
}

/*
	ANT_COMPRESS_TEXT_DEFLATE::DECOMPRESS()
	---------------------------------------
*/
char *ANT_compress_text_deflate::decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_ZLIB
	unsigned long status;

	if (deflateReset(&internals->stream) != Z_OK)
		return NULL;

	internals->stream.avail_in = source_length;
	internals->stream.next_in = (Bytef *)source;
	internals->stream.avail_out = *destination_length;
	internals->stream.next_out = (Bytef *)destination;

	status = inflate(&internals->stream, Z_FINISH);
	*destination_length = *destination_length - internals->stream.avail_out;

	return status == Z_STREAM_END ? destination : NULL;
#else
	return NULL;
#endif
}

