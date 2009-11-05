/*
	COMPRESS_TEXT_NONE.C
	--------------------
*/
#include <string.h>
#include "compress_text_none.h"

/*
	ANT_COMPRESS_TEXT_NONE::COMPRESS()
	----------------------------------
*/
char *ANT_compress_text_none::compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
if (*destination_length < source_length)
	return NULL;

return (char *)memcpy(destination, source, *destination_length = source_length);
}

/*
	ANT_COMPRESS_TEXT_NONE::DECOMPRESS()
	------------------------------------
*/
char *ANT_compress_text_none::decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length)
{
if (*destination_length < source_length)
	return NULL;

return (char *)memcpy(destination, source, *destination_length = source_length);
}
