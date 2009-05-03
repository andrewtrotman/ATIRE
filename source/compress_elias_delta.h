/*
	COMPRESS_ELIAS_DELTA.H
	----------------------
*/
#ifndef __COMPRESS_ELIAS_DELTA_H__
#define __COMPRESS_ELIAS_DELTA_H__

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

#endif __COMPRESS_ELIAS_DELTA_H__
