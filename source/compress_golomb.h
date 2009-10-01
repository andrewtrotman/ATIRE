/*
	COMPRESS_GOLOMB.H
	-----------------
*/
#ifndef COMPRESS_GOLOMB_H_
#define COMPRESS_GOLOMB_H_

#include "compress_elias_delta.h"

/*
	class ANT_COMPRESS_GOLOMB
	-------------------------
*/
class ANT_compress_golomb : public ANT_compress_elias_delta
{
protected:
	long long factor;
	long log2_factor;
	unsigned long long pivot;

protected:
	long long compute_golomb_factor(ANT_compressable_integer *position, long long count);
	void set_factor(long long factor);

	inline void encode(unsigned long long val);
	inline unsigned long long decode(void);

public:
	ANT_compress_golomb() {}
	virtual ~ANT_compress_golomb() {}

	long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* COMPRESS_GOLOMB_H_ */

