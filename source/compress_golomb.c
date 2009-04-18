/*
	COMPRESS_GOLOMB.C
	-----------------
*/
#include "compress_golomb.h"
#include "maths.h"

/*
	ANT_COMPRESS_GOLOMB::COMPUTE_GOLOMB_FACTOR()
	--------------------------------------------
*/
long long ANT_compress_golomb::compute_golomb_factor(ANT_compressable_integer *position, long long count)
{
ANT_compressable_integer *from;
long long x, factor;
unsigned long long sum;

for (from = position, sum = x = 0; x < count; x++, from++)
	sum += *from;
if ((factor = (long long)(0.69 * sum / count)) < 1)
	factor = 1;

return factor;
}

/*
	ANT_COMPRESS_GOLOMB::SET_FACTOR()
	---------------------------------
*/
void ANT_compress_golomb::set_factor(long long factor)
{
this->factor = factor;
log2_factor = ANT_floor_log2(factor);
pivot = ((unsigned long long)1 << (log2_factor + 1)) - factor;
}

/*
	ANT_COMPRESS_GOLOMB::ENCODE()
	-----------------------------
*/
inline void ANT_compress_golomb::encode(unsigned long long val)
{
unsigned long long man;
unsigned long long exp = val / factor;

bitstream.push_zeros(exp);
bitstream.push_one();

man = val - exp * factor;

if (man < pivot)
	bitstream.push_bits(man, log2_factor);
else
	bitstream.push_bits(pivot + man, log2_factor + 1);
}

/*
	ANT_COMPRESS_GOLOMB::DECODE()
	-----------------------------
*/
inline unsigned long long ANT_compress_golomb::decode(void)
{
unsigned long long man;
unsigned long long exp = 0;

while (bitstream.get_bit() == 0)
	exp++;

man = bitstream.get_bits(log2_factor);
if (man >= pivot)
	man = ((man << 1) | bitstream.get_bit()) - pivot;

return man + exp * factor;
}

/*
	LONG ANT_COMPRESS_GOLOMB::COMPRESS()
	------------------------------------
*/
long long ANT_compress_golomb::compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers)
{
set_factor(compute_golomb_factor(source, source_integers));
bitstream.rewind(destination, destination_length);
ANT_compress_elias_delta::encode(factor);				// store the factor first so that we can decompress later
while (source_integers-- > 0)
	encode(*source++);
return eof();
}

/*
	ANT_COMPRESS_GOLOMB::DECOMPRESS()
	---------------------------------
*/
void ANT_compress_golomb::decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers)
{
bitstream.rewind(source, 0);
set_factor(ANT_compress_elias_delta::decode());		// read the factor first so that we can then decode
while (destination_integers-- > 0)
	*destination++ = (ANT_compressable_integer)decode();
}
