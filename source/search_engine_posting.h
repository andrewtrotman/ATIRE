/*
	SEARCH_ENGINE_POSTING.H
	-----------------------
*/
#ifndef __SEARCH_ENGINE_POSTING_H__
#define __SEARCH_ENGINE_POSTING_H__

#include "compress.h"
/*
	class ANT_SEARCH_ENGINE_POSTING
	-------------------------------
*/
class ANT_search_engine_posting
{
public:
	ANT_compressable_integer *docid;
	ANT_compressable_integer *tf;
} ;

#endif __SEARCH_ENGINE_POSTING_H__
