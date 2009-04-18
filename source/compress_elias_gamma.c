/*
	COMPRESS_ELIAS_GAMMA.C
	----------------------
*/
#include "maths.h"
#include "compress_elias_gamma.h"

/*
	ANT_COMPRESS_ELIAS_GAMMA::ENCODE()
	---------------------------------
*/
inline void ANT_compress_elias_gamma::encode(unsigned long long val)
{
long exp = ANT_floor_log2(++val);

bitstream.push_zeros(exp);
bitstream.push_bits(val, exp + 1);
}

/*
	ANT_COMPRESS_ELIAS_GAMMA::DECODE()
	-----------------------------------
*/
inline unsigned long long ANT_compress_elias_gamma::decode(void)
{
long exp = 0;

while (bitstream.get_bit() == 0)
	exp++;

return ((((unsigned long long)1) << exp) | bitstream.get_bits(exp)) - 1;
}

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

