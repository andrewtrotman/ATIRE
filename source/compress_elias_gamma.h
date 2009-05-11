
/*
	COMPRESS_ELIAS_GAMMA.H
	----------------------
*/
#ifndef __COMPRESS_ELIAS_GAMMA_H__
#define __COMPRESS_ELIAS_GAMMA_H__

#include "maths.h"
#include "compress.h"
#include "bitstream.h"

/*
	class ANT_COMPRESS_ELIAS_GAMMA
	------------------------------
*/
class ANT_compress_elias_gamma : public ANT_compress
{
protected:
	ANT_bitstream bitstream;

protected:
    inline void encode(unsigned long long val) 
        {
        long exp = ANT_floor_log2(++val);

        bitstream.push_zeros(exp);
        bitstream.push_bits(val, exp + 1);
        }

	inline unsigned long long decode(void)
        {
        long exp = 0;

        while (bitstream.get_bit() == 0)
	        exp++;

        return ((((unsigned long long)1) << exp) | bitstream.get_bits(exp)) - 1;
        }

	long long eof(void) { return bitstream.eof(); }

public:
	ANT_compress_elias_gamma() {}
	virtual ~ANT_compress_elias_gamma() {};

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif __COMPRESS_ELIAS_GAMMA_H__

