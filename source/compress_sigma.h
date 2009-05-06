/*
	COMPRESS_SIGMA.H
	----------------
*/

#ifndef __COMPRESS_SIGMA_H__
#define __COMPRESS_SIGMA_H__

#include "compress.h"

/*
	class ANT_COMPRESS_SIGMA
	------------------------
*/
class ANT_compress_sigma : public ANT_compress
{
public:
	ANT_compress_sigma() {}
	virtual ~ANT_compress_sigma() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);

} ;



#endif __COMPRESS_SIGMA_H__
