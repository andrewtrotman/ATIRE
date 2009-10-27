/*
	COMPRESS_TEXT_BZ2.C
	-------------------
*/
#ifdef ANT_HAS_BZLIB
	#include "../bzip/bzip2-1.0.5/bzlib.h"
#endif
#include "compress_text_bz2.h"

/*
	ANT_COMPRESS_TEXT_BZ2::COMPRESS()
	---------------------------------
*/
char *ANT_compress_text_bz2::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_BZLIB
	unsigned int bytes = (unsigned int)*destination_length;
	if (BZ2_bzBuffToBuffCompress(destination, &bytes, source, source_length, 9, 0, 0) == BZ_OK)
		{
		*destination_length = bytes;
		return destination;
		}
#endif
return NULL;

}

/*
	ANT_COMPRESS_TEXT_BZ2::DECOMPRESS()
	-----------------------------------
*/
char *ANT_compress_text_bz2::decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
#ifdef ANT_HAS_BZLIB
	unsigned int bytes = (unsigned int)*destination_length;
	if (BZ2_bzBuffToBuffDecompress(destination, &bytes, source, source_length, 0, 0) == BZ_OK)
		{
		*destination_length = bytes;
		return destination;
		}
#endif
return NULL;
}
