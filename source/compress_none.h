/*
	COMPRESS_NONE.H
	---------------
*/
#ifndef COMPRESS_NONE_H_
#define COMPRESS_NONE_H_

#include "compress.h"

/*
	class ANT_COMPRESS_NONE
	-----------------------
*/
class ANT_compress_none : public ANT_compress
{
public:
	ANT_compress_none() {}
	virtual ~ANT_compress_none() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
} ;

#endif  /* COMPRESS_NONE_H_ */
