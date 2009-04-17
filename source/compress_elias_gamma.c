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
inline void ANT_compress_elias_gamma::encode(uint32_t val)
{
long exp = ANT_floor_log2(++val);

bitstream.push_bits(0, exp);
bitstream.push_bits(val, exp + 1);
}

/*
	ANT_COMPRESS_ELIAS_GAMMA::DECODE()
	-----------------------------------
*/
inline uint32_t ANT_compress_elias_gamma::decode(void)
{
long exp = 0;

while (bitstream.get_bit() == 0)
	exp++;

return ((((uint32_t)1) << exp) | bitstream.get_bits(exp)) - 1;
}

/*
	ANT_COMPRESS_ELIAS_GAMMA::COMPRESS()
	------------------------------------
*/
long ANT_compress_elias_gamma::compress(unsigned char *destination, unsigned long destination_length, uint32_t *source, unsigned long source_integers)
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
void ANT_compress_elias_gamma::decompress(uint32_t *destination, unsigned char *source, unsigned long destination_integers)
{
bitstream.rewind(source, 0);
while (destination_integers-- > 0)
	*destination++ = decode();
}

