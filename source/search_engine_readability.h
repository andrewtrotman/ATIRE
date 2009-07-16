/*
	SEARCH_ENGINE_READABILITY.H
	---------------------------
*/
#ifndef __SEARCH_ENGINE_READABILITY_H__
#define __SEARCH_ENGINE_READABILITY_H__

#include "search_engine.h"

class ANT_memory;
class ANT_search_engine_btree_leaf;

/*
	class ANT_SEARCH_ENGINE_READABILITY
	-----------------------------------
*/
class ANT_search_engine_readability : public ANT_search_engine
{
private:
	long *document_readability;
	long hardest_document;

protected:
	void relevance_rank(ANT_search_engine_btree_leaf *leaf, ANT_search_engine_posting *postings);

public:
	ANT_search_engine_readability(ANT_memory *memory);
	virtual ~ANT_search_engine_readability() {};

};

#endif __SEARCH_ENGINE_READABILITY_H__
