/*
	SEARCH_ENGINE_POSTING.H
	-----------------------
*/
#ifndef SEARCH_ENGINE_POSTING_H_
#define SEARCH_ENGINE_POSTING_H_

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

#endif  /* SEARCH_ENGINE_POSTING_H_ */
