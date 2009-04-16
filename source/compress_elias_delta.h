/*
	COMPRESS_ELIAS_DELTA.H
	----------------------
*/
#ifndef __COMPRESS_ELIAS_DELTA_H__
#define __COMPRESS_ELIAS_DELTA_H__

#include "bitstream.h"
#include "compress_elias_gamma.h"
#include "log2.h"

/*
	class ANT_COMPRESS_ELIAS_DELTA
	------------------------------
*/
class ANT_compress_elias_delta : public ANT_compress_elias_gamma
{
public:
	ANT_compress_elias_delta() {}
	virtual ~ANT_compress_elias_delta() {}

	inline void encode(unsigned long val);
	inline unsigned long decode(void);
	inline void decode(unsigned long *list, long len) { while (len-- > 0) *list++ = decode(); }
} ;

/*
	ANT_COMPRESS_ELIAS_DELTA::ENCODE()
	----------------------------------
*/
inline void ANT_compress_elias_delta::encode(unsigned long val)
{
long exp = ANT_log2(++val);

ANT_compress_elias_gamma::encode(exp + 1);
bitstream->push_bits(val, exp);
}

/*
	ANT_COMPRESS_ELIAS_DELTA::DECODE()
	----------------------------------
*/
inline unsigned long ANT_compress_elias_delta::decode(void)
{
unsigned long exp = ANT_compress_elias_gamma::decode() - 1;

return ((((unsigned long)1) << exp) | bitstream->get_bits(exp)) - 1;
}

#endif __COMPRESS_ELIAS_DELTA_H__
