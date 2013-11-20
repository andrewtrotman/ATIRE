/*
	COMPRESSION_TEXT_FACTORY_SCHEME.H
	---------------------------------
*/
#ifndef COMPRESSION_TEXT_FACTORY_SCHEME_H_
#define COMPRESSION_TEXT_FACTORY_SCHEME_H_

#include "compress_text.h"

/*
	class ANT_COMPRESSION_TEXT_FACTORY_SCHEME
	-----------------------------------------
*/
class ANT_compression_text_factory_scheme
{
public:
	long scheme_id;	// the external name of the scheme as a bit pattern.  Done so the user can select which schemes to use
	ANT_compress_text *scheme;	// ANT_compress_text object
	const char *name;			// the name of the scheme
} ;


#endif /* COMPRESSION_TEXT_FACTORY_SCHEME_H_ */
