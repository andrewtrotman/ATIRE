/*
	COMPRESS_ELIAS_DELTA.C
	----------------------
*/
#include "compress_elias_delta.h"
#include "maths.h"


/*
	ANT_COMPRESS_ELIAS_DELTA::COMPRESS()
	------------------------------------
*/
long long ANT_compress_elias_delta::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
bitstream.rewind(destination, destination_length);
while (source_integers-- > 0)
	encode(*source++);
return eof();
}

/*
	ANT_COMPRESS_ELIAS_DELTA::DECOMPRESS()
	--------------------------------------
*/
void ANT_compress_elias_delta::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
bitstream.rewind(source, 0);
while (destination_integers-- > 0)
	*destination++ = (ANT_compressable_integer)decode();
}

