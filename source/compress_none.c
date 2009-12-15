/*
	COMPRESS_NONE.C
	---------------
*/
#include <stdio.h>
#include <string.h>
#include "compress_none.h"

/*
	ANT_COMPRESS_NONE::COMPRESS()
	-----------------------------
*/
long long ANT_compress_none::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
long long bytes;

bytes = source_integers * sizeof(ANT_compressable_integer);
if (bytes > destination_length)
	return 0;

memmove(destination, source, (size_t)bytes);
return bytes;
}

/*
	ANT_COMPRESS_NONE::DECOMPRESS()
	-------------------------------
*/
void ANT_compress_none::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
memmove(destination, source, (size_t)(destination_integers * sizeof(ANT_compressable_integer)));
}

