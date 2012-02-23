/*
	SEARCH_ENGINE_MEMORY_INDEX.H
	----------------------------
*/
#ifndef SEARCH_ENGINE_MEMORY_INDEX_H_
#define SEARCH_ENGINE_MEMORY_INDEX_H_

#include "search_engine.h"

class ANT_memory_index;

/*
	class ANT_SEARCH_ENGINE_MEMORY_INDEX
	------------------------------------
*/
class ANT_search_engine_memory_index : public ANT_search_engine
{
protected:
	ANT_memory_index *index;

public:
	ANT_search_engine_memory_index(ANT_memory_index *index, ANT_memory *memory);
	virtual ~ANT_search_engine_memory_index();

	virtual unsigned char *get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination);
	virtual ANT_search_engine_btree_leaf *get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details);
} ;

#endif /* SEARCH_ENGINE_MEMORY_INDEX_H_ */

