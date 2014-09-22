/*
	COMPRESS_SIMPLE8B.H
	------------------
*/
#ifndef __COMPRESS_SIMPLE8B_H__
#define __COMPRESS_SIMPLE8B_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIMPLE8B
	--------------------------
*/
class ANT_compress_simple8b : public ANT_compress
{
protected:
	static const long bits_to_use[];
	static const unsigned char bits_used_table[];
	static const unsigned char ints_packed_table[];

public:
	ANT_compress_simple8b() {}
	virtual ~ANT_compress_simple8b() {}

	virtual void pack(ANT_compressable_integer *source, uint64_t *dest, uint32_t mask_type, uint32_t num_to_pack);
	virtual long can_pack(ANT_compressable_integer *from, uint32_t mask_type, uint32_t pack_limit);
	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* __COMPRESS_SIMPLE8B_H__ */
