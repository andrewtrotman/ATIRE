/*
	COMPRESS_ELIAS_GAMMA.C
	----------------------
*/
#include "maths.h"
#include "compress_elias_gamma.h"


/*
	ANT_COMPRESS_ELIAS_GAMMA::COMPRESS()
	------------------------------------
*/
long long ANT_compress_elias_gamma::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
bitstream.rewind(destination, destination_length);
while (source_integers-- > 0)
	encode(*source++);
return eof();
}

/*
	ANT_COMPRESS_ELIAS_GAMMA::DECOMPRESS()
	--------------------------------------
*/
void ANT_compress_elias_gamma::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
bitstream.rewind(source, 0);
while (destination_integers-- > 0)
	*destination++ = (ANT_compressable_integer)decode();
}

