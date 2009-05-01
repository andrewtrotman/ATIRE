/*
	COMPRESS_CARRYOVER12.H
	----------------------
*/
#ifndef __COMPRESS_CARRYOVER12_H__
#define __COMPRESS_CARRYOVER12_H__

#include "compress.h"

class ANT_compress_carryover12 : ANT_compress
{
public:
	ANT_compress_carryover12(long long max_list_length) : ANT_compress(max_list_length) {}
	virtual ~ANT_compress_carryover12() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;


#endif __COMPRESS_CARRYOVER12_H__

