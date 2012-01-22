/*
	COMPRESS_TEXT_SNAPPY.C
	----------------------
*/
#include <cstddef>

#ifdef ANT_HAS_SNAPPYLIB
	#include "snappy.h"
#endif
#include "compress_text_snappy.h"


/*
	ANT_COMPRESS_TEXT_SNAPPY::SPACE_NEEDED_TO_COMPRESS()
	----------------------------------------------------
	what is the minimum amount of space required to pass or fail a compression attempt
*/
unsigned long ANT_compress_text_snappy::space_needed_to_compress(unsigned long source_length)
{
#ifdef ANT_HAS_SNAPPYLIB

	return (unsigned long)snappy::MaxCompressedLength((unsigned long)source_length);
#else
	return 0;
#endif
}

/*
	ANT_COMPRESS_TEXT_SNAPPY::COMPRESS()
	------------------------------------
*/
char *ANT_compress_text_snappy::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_SNAPPYLIB

	size_t bytes;

	snappy::RawCompress(source, source_length, destination, &bytes);

	if (bytes > *destination_length)
		return NULL;		// compresson would make it larger!!!

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
