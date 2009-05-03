
/*
	COMPRESS_ELIAS_GAMMA.H
	----------------------
*/
#ifndef __COMPRESS_ELIAS_GAMMA_H__
#define __COMPRESS_ELIAS_GAMMA_H__

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
	inline void encode(unsigned long long val);
	inline unsigned long long decode(void);
	long long eof(void) { return bitstream.eof(); }

public:
	ANT_compress_elias_gamma() {}
	virtual ~ANT_compress_elias_gamma() {};

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif __COMPRESS_ELIAS_GAMMA_H__

