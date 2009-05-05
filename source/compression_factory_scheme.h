/*
	COMPRESSION_FACTORY_SCHEME.H
	----------------------------
*/

#ifndef __COMPRESSION_FACTORY_SCHEME_H__
#define __COMPRESSION_FACTORY_SCHEME_H__

#include "compress.h"

/*
	class ANT_COMPRESSION_FACTORY_SCHEME
	------------------------------------
*/
class ANT_compression_factory_scheme
{
public:
	ANT_compress *scheme;		// ANT_compress object
	char *name;					// name of the scheme
	long long uses;				// number of times this compression scheme was chosen as the "best" scheme
	long long would_take;		// the number of bytes it would take if only this scheme were used
	long long did_take;			// the number of bytes this scheme did take (for just the this->uses cases)
} ;


#endif __COMPRESSION_FACTORY_SCHEME_H__
