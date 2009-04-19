/*
	COMPRESS_SIMPLE9.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE9_H__
#define __COMPRESS_SIMPLE9_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIMPLE9
	--------------------------
*/
class ANT_compress_simple9 : ANT_compress
{
protected:
	class ANT_compress_simple9_lookup
		{
		public:
			long long numbers;
			long bits;
			long mask;
		} ;
protected:
	static ANT_compress_simple9_lookup simple9_table[];

protected:
	long ANT_compress_simple9::DoesHighestFit(ANT_compressable_integer d[], long pos, long noOfDigits, long noOfBits, long size);

public:
	ANT_compress_simple9(long long max_list_length) : ANT_compress(max_list_length) {}
	virtual ~ANT_compress_simple9() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif __COMPRESS_SIMPLE9_H__
