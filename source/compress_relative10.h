/*
	COMPRESS_RELATIVE10.H
	---------------------
*/
#ifndef __COMPRESS_RELATIVE10_H__
#define __COMPRESS_RELATIVE10_H__

#include "compress_simple9.h"

/*
	class ANT_COMPRESS_RELATIVE10
	-----------------------------
*/
class ANT_compress_relative10 : ANT_compress_simple9
{
public:
	ANT_compress_relative10(long long max) : ANT_compress_simple9(max) {}
	virtual ~ANT_compress_relative10() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif __COMPRESS_RELATIVE10_H__
