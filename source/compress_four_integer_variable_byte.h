/*
	COMPRESS_FOUR_INTEGER_VARIABLE_BYTE.H
	------------------
*/
#ifndef __COMPRESS_FOUR_INTEGER_VARIABLE_BYTE_H__
#define __COMPRESS_FOUR_INTEGER_VARIABLE_BYTE_H__

#include "compress.h"

/*
	class ANT_COMPRESS_FOUR_INTEGER_VARIABLE_BYTE
	--------------------------
*/
class ANT_compress_four_integer_variable_byte : public ANT_compress
{
public:
	ANT_compress_four_integer_variable_byte() {}
	virtual ~ANT_compress_four_integer_variable_byte() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* __COMPRESS_FOUR_INTEGER_VARIABLE_BYTE_H__ */
