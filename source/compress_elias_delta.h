/*
	COMPRESS_ELIAS_DELTA.H
	----------------------
*/
#ifndef COMPRESS_ELIAS_DELTA_H_
#define COMPRESS_ELIAS_DELTA_H_

#include "compress_elias_gamma.h"

/*
	class ANT_COMPRESS_ELIAS_DELTA
	------------------------------
*/
class ANT_compress_elias_delta : public ANT_compress_elias_gamma
{
protected:
	inline void encode(unsigned long long val);
	inline unsigned long long decode(void);

public:
	ANT_compress_elias_delta() {}
	virtual ~ANT_compress_elias_delta() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;


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

#endif  /* COMPRESS_ELIAS_DELTA_H_ */
