/*
	COMPRESS_GOLOMB.H
	-----------------
*/
#ifndef __COMPRESS_GOLOMB_H__
#define __COMPRESS_GOLOMB_H__

#include "bitstream.h"

/*
	class ANT_COMPRESS_GOLOMB
	-------------------------
*/
class ANT_compress_golomb
{
protected:
	ANT_bitstream *bitstream;
	long factor, log2_factor;
	unsigned long pivot;

public:
	ANT_compress_golomb(long p_factor);
	virtual ~ANT_compress_golomb();

	inline void encode(unsigned long val);
	inline unsigned long decode(void);

	void decode(unsigned long *list, long len) { while (len-- > 0) *list++ = decode(); }

	long eof(void) { return bitstream->eof(); }
	void text_render(void) { bitstream->text_render(); }
} ;

/*
	ANT_COMPRESS_GOLOMB::ENCODE()
	-----------------------------
*/
inline void ANT_compress_golomb::encode(unsigned long val)
{
unsigned long man;
long exp = (long)(val / factor);

bitstream->push_bits(0, exp);
bitstream->push_one();

man = val - exp * factor;

if (man < pivot)
	bitstream->push_bits(man, log2_factor);
else
	bitstream->push_bits(pivot + man, log2_factor + 1);
}

/*
	ANT_COMPRESS_GOLOMB::DECODE()
	-----------------------------
*/
inline unsigned long ANT_compress_golomb::decode(void)
{
unsigned long man;
long exp = 0;

while (bitstream->get_bit() == 0)
	exp++;

man = bitstream->get_bits(log2_factor);
if (man >= pivot)
	man = ((man << 1) | bitstream->get_bit()) - pivot;

return man + exp * factor;
}

#endif __COMPRESS_GOLOMB_H__
