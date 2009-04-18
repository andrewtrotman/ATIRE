/*
	COMPRESS_GOLOMB.H
	-----------------
*/
#ifndef __COMPRESS_GOLOMB_H__
#define __COMPRESS_GOLOMB_H__

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
	ANT_compress_golomb(long long max_list_length) : ANT_compress_elias_delta(max_list_length) {}
	virtual ~ANT_compress_golomb() {}

	long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif __COMPRESS_GOLOMB_H__

