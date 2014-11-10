/*
	COMPRESSION_FACTORY_SCHEME.H
	----------------------------
*/

#ifndef COMPRESSION_FACTORY_SCHEME_H_
#define COMPRESSION_FACTORY_SCHEME_H_

#include "compress.h"

/*
	class ANT_COMPRESSION_FACTORY_SCHEME
	------------------------------------
*/
class ANT_compression_factory_scheme
{
public:
	unsigned long scheme_id;	// the external name of the scheme as a bit pattern.  Done so the user can select which schemes to use
	ANT_compress *scheme;		// ANT_compress object
	const char *name;					// the name of the scheme
	long long uses;				// the number of times this compression scheme was chosen as the "best" scheme
	long long would_take;		// the number of bytes it would take if only this scheme were used
	long long did_take;			// the number of bytes this scheme did take (for just the this->uses cases)
	long long did_compress;		// the number of integers that were compressed using this scheme
	long long failures;			// the number of times this scheme resulted in a list longer than the raw list
	long long compress_time;				// the time required to compress all the postings lists (in ANT_stats frequency units)
	long long decompress_time;				// the time required to decompress all the postings lists (in ANT_stats frequency units)
} ;


#endif  /* COMPRESSION_FACTORY_SCHEME_H_ */
