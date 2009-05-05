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
	ANT_compress *scheme;
	char *name;
	long long uses;
	long long times;
	long long bytes;
} ;


#endif __COMPRESSION_FACTORY_SCHEME_H__
