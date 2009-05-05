/*
	COMPRESSION_FACTORY.H
	---------------------
*/
#ifndef __COMPRESSION_FACTORY_H__
#define __COMPRESSION_FACTORY_H__

#include <string.h>
#include "compress.h"

/*
	class ANT_COMPRESSION_FACTORY
	-----------------------------
*/
class ANT_compression_factory : ANT_compress
{
private:
	static long number_of_techniques;
	
public:
	ANT_compression_factory();
	virtual ~ANT_compression_factory() {}

	virtual long long compress(unsigned char *destination, long long destination_length, ANT_compressable_integer *source, long long source_integers);
	virtual void decompress(ANT_compressable_integer *destination, unsigned char *source, long long destination_integers);
										
	void text_render(void);

} ;

#endif __COMPRESSION_FACTORY_H__
