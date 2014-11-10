/*
	COMPRESS_SIMPLE16.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE16_H__
#define __COMPRESS_SIMPLE16_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIMPLE16
	--------------------------
*/
class ANT_compress_simple16 : public ANT_compress
{
protected:
	static long ints_packed_table[];
	static long can_pack_table[];
	static long row_for_bits_needed[];
	static long invalid_masks_for_offset[];
	static long simple16_shift_table[];

public:
	ANT_compress_simple16() {}
	virtual ~ANT_compress_simple16() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* __COMPRESS_SIMPLE16_H__ */
