
/*
	COMPRESS_ELIAS_GAMMA.H
	----------------------
*/
#ifndef __COMPRESS_ELIAS_GAMMA_H__
#define __COMPRESS_ELIAS_GAMMA_H__

#include "bitstream.h"
#include "log2.h"

/*
	class ANT_ELIAS
	---------------
*/
class ANT_compress_elias_gamma
{
protected:
	ANT_bitstream *bitstream;

public:
	ANT_compress_elias_gamma();
	virtual ~ANT_compress_elias_gamma();

	inline void encode(unsigned long val);
	inline unsigned long decode(void);
	inline void decode(unsigned long *list, long len) { while (len-- > 0) *list++ = decode(); }

	long eof(void) { return bitstream->eof(); }
	void text_render(void) { bitstream->text_render(); }
} ;

/*
	ANT_COMPRESS_ELIAS_GAMMA::ENCODE()
	---------------------------------
*/
inline void ANT_compress_elias_gamma::encode(unsigned long val)
{
long exp = ANT_log2(++val);

bitstream->push_bits(0, exp);
bitstream->push_bits(val, exp + 1);
}

/*
	ANT_COMPRESS_ELIAS_GAMMA::DECODE()
	-----------------------------------
*/
inline unsigned long ANT_compress_elias_gamma::decode(void)
{
long exp = 0;

while (bitstream->get_bit() == 0)
	exp++;

return ((((unsigned long)1) << exp) | bitstream->get_bits(exp)) - 1;
}

#endif __COMPRESS_ELIAS_GAMMA_H__
