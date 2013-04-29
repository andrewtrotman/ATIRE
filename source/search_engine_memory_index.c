/*
	SEARCH_ENGINE_MEMORY_INDEX.C
	----------------------------
*/
#include "btree_iterator.h"
#include "search_engine_memory_index.h"
#include "search_engine_btree_leaf.h"
#include "memory_index.h"

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::ANT_SEARCH_ENGINE_MEMORY_INDEX()
	----------------------------------------------------------------
*/
ANT_search_engine_memory_index::ANT_search_engine_memory_index(ANT_memory_index *index, ANT_memory *memory) : ANT_search_engine(memory)
{
this->index = index;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::~ANT_SEARCH_ENGINE_MEMORY_INDEX()
	-----------------------------------------------------------------
*/
ANT_search_engine_memory_index::~ANT_search_engine_memory_index()
{
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::GET_POSTINGS()
	----------------------------------------------
*/
unsigned char *ANT_search_engine_memory_index::get_postings(ANT_search_engine_btree_leaf *term_details, unsigned char *destination)
{
ANT_memory_index_hash_node *index_node;
long long length;

index_node = (ANT_memory_index_hash_node *)term_details->postings_position_on_disk;

length = index->node_to_postings(index_node);
memcpy(destination, index->impacted_postings, (size_t)(length * sizeof(*index->impacted_postings)));

return destination;
}

/*
	ANT_SEARCH_ENGINE_MEMORY_INDEX::GET_POSTINGS_DETAILS()
	------------------------------------------------------
*/
ANT_search_engine_btree_leaf *ANT_search_engine_memory_index::get_postings_details(char *term, ANT_search_engine_btree_leaf *term_details)
{
ANT_memory_index_hash_node *index_node;
ANT_string_pair term_as_pair(term);
long hash_of_term = index->hash(&term_as_pair);

if (index->hash_table[hash_of_term] == NULL)
	return NULL;			// term is not in the index

index_node = index->find_node(index->hash_table[hash_of_term], &term_as_pair);

if (index_node == NULL)
	return NULL;

term_details->global_collection_frequency = term_details->local_collection_frequency = index_node->collection_frequency;
term_details->global_document_frequency = term_details->local_document_frequency = index_node->document_frequency;
term_details->postings_position_on_disk = (long long)index_node;		// this is a dreadful hack, it assumes a pointer will fit in a 64-bit pointer and actually points to the index_node not the postings
/*
	We fake the next three because we just don't know.
*/
term_details->impacted_length = 0;
term_details->postings_length = 0;
#ifdef TERM_LOCAL_MAX_IMPACT
	term_details->local_max_impact = 128;  // Without serialising the postings list we just don't know.  
#endif
return term_details;
}
