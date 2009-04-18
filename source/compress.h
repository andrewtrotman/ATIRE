/*
	COMPRESS.H
	----------
*/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "fundamental_types.h"

typedef uint32_t ANT_compressable_integer;
/*
	class ANT_COMPRESS
	------------------
*/
class ANT_compress
{
protected:
	long long length_of_longest_possible_list;

public:
	ANT_compress(long long max_list_length) { length_of_longest_possible_list = max_list_length; }
	virtual ~ANT_compress() {}

	/*
		destination_length is in bytes.  source_integers is in units of integers, returns the length in bytes
	*/
	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers) = 0;
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers) = 0;
} ;

#endif __COMPRESS_H__

