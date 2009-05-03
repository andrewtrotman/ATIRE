/*
	COMPRESS_VARIABLE_BYTE.H
	------------------------
*/
#ifndef __COMPRESS_VARIABLE_BYTE_H__
#define __COMPRESS_VARIABLE_BYTE_H__

#include "compress.h"

/*
	class ANT_COMPRESS_VARIABLE_BYTE
	--------------------------------
*/
class ANT_compress_variable_byte : public ANT_compress
{
public:
	ANT_compress_variable_byte() {}
	virtual ~ANT_compress_variable_byte() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);

	static inline long compress_bytes_needed(long long docno);
	static inline void compress_into(unsigned char *dest, long long docno);
} ;

/*
	ANT_COMPRESS_VARIABLE_BYTE::COMPRESS_BYTES_NEEDED()
	---------------------------------------------------
*/
inline long ANT_compress_variable_byte::compress_bytes_needed(long long docno)
{
if (docno & ((long long)127 << 28))
	return 5;
else if (docno & ((long long)127 << 21))
	return 4;
else if (docno & ((long long)127 << 14))
	return 3;
else if (docno & ((long long)127 << 7))
	return 2;
else
	return 1;
}

/*
	ANT_COMPRESS_VARIABLE_BYTE::COMPRESS_INTO()
	-------------------------------------------
*/
inline void ANT_compress_variable_byte::compress_into(unsigned char *dest, long long docno)
{
if (docno & ((long long)127 << 28))
	goto five;
else if (docno & ((long long)127 << 21))
	goto four;
else if (docno & ((long long)127 << 14))
	goto three;
else if (docno & ((long long)127 << 7))
	goto two;
else
	goto one;

five:
	*dest++ = (docno >> 28) & 0x7F;
four:
	*dest++ = (docno >> 21) & 0x7F;
three:
	*dest++ = (docno >> 14) & 0x7F;
two:
	*dest++ = (docno >> 7) & 0x7F;
one:
	*dest++ = (docno & 0x7F) | 0x80;
}

#endif __COMPRESS_VARIABLE_BYTE_H__
