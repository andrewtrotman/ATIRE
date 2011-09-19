/*
	COMPRESS_TEXT_SNAPPY.C
	----------------------
*/
#ifdef ANT_HAS_SNAPPYLIB
	#include "../snappy/snappy-1.0.4/snappy.h"
#endif
#include "compress_text_snappy.h"

/*
	ANT_COMPRESS_TEXT_SNAPPY::COMPRESS()
	------------------------------------
*/
char *ANT_compress_text_snappy::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_SNAPPYLIB

	size_t bytes;

	snappy::RawCompress(source, source_length, destination, &bytes);
	*destination_length = (unsigned long)bytes;
	return destination;

#else

	return NULL;

#endif
}

/*
	ANT_COMPRESS_TEXT_SNAPPY::DECOMPRESS()
	--------------------------------------
*/
char *ANT_compress_text_snappy::decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_SNAPPYLIB
	size_t length;

	if (snappy::RawUncompress(source, source_length, destination))
		{
		snappy::GetUncompressedLength(source, source_length, &length);
		*destination_length = (unsigned long)length;
		return destination;
		}
#endif

return NULL;
}
