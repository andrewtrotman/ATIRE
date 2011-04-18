/*
	SEARCH_ENGINE_READABILITY.H
	---------------------------
*/
#ifndef SEARCH_ENGINE_READABILITY_H_
#define SEARCH_ENGINE_READABILITY_H_

#include "search_engine.h"

class ANT_memory;
class ANT_search_engine_btree_leaf;

/*
	class ANT_SEARCH_ENGINE_READABILITY
	-----------------------------------
*/
class ANT_search_engine_readability : public ANT_search_engine
{
friend class ANT_ranking_function_readability;

private:
	long *document_readability;
	long hardest_document;

private:
	void initialise(ANT_memory *memory);
	virtual int open(const char *filename = "index.aspt");
public:
	ANT_search_engine_readability(ANT_memory *memory, long memory_model) : ANT_search_engine(memory, memory_model) {}
	virtual ~ANT_search_engine_readability() {};
};

#endif  /* SEARCH_ENGINE_READABILITY_H_ */
