/*
	COMPRESS_ELIAS_DELTA.C
	----------------------
*/
#include "compress_elias_delta.h"
#include "maths.h"

/*
	ANT_COMPRESS_ELIAS_DELTA::ENCODE()
	----------------------------------
*/
inline void ANT_compress_elias_delta::encode(unsigned long long val)
{
long exp = ANT_floor_log2(++val);

ANT_compress_elias_gamma::encode(exp + 1);
bitstream.push_bits(val, exp);
}

/*
	ANT_COMPRESS_ELIAS_DELTA::DECODE()
	----------------------------------
*/
inline unsigned long long ANT_compress_elias_delta::decode(void)
{
long exp = (long)ANT_compress_elias_gamma::decode() - 1;

return ((((unsigned long long)1) << exp) | bitstream.get_bits(exp)) - 1;
}

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

