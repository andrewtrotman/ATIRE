
/*
	COMPRESS_ELIAS_GAMMA.H
	----------------------
*/
#ifndef __COMPRESS_ELIAS_GAMMA_H__
#define __COMPRESS_ELIAS_GAMMA_H__

#include "compress.h"
#include "bitstream.h"
#include "maths.h"

/*
	class ANT_COMPRESS_ELIAS_GAMMA
	------------------------------
*/
class ANT_compress_elias_gamma : public ANT_compress
{
protected:
	ANT_bitstream bitstream;

private:
	inline void encode(uint32_t val);
	inline uint32_t decode(void);
	unsigned long eof(void) { return bitstream.eof(); }

public:
	ANT_compress_elias_gamma(unsigned long max_list_length) : ANT_compress(max_list_length) {};
	virtual ~ANT_compress_elias_gamma() {};

	virtual long compress(unsigned char *destination, unsigned long destination_length, uint32_t *source, unsigned long source_integers);
	virtual void decompress(uint32_t *destination, unsigned char *source, unsigned long destination_integers);
} ;

#endif __COMPRESS_ELIAS_GAMMA_H__

