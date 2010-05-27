/*
	DIRECTORY_ITERATOR_PREINDEX.H
	-----------------------------
*/
#ifndef DIRECTORY_ITERATOR_PREINDEX_H_
#define DIRECTORY_ITERATOR_PREINDEX_H_

#include "directory_iterator.h"
#include "directory_iterator_object.h"
#include "producer_consumer.h"

class ANT_indexer_param_block;
class ANT_memory_indexer;
class ANT_memory_index;
class ANT_directory_iterator_preindex_internals;
class ANT_directory_iterator_object;
class ANT_stem;
class ANT_readability_factory;

typedef long (*ANT_index_document)(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file);

/*
	class ANT_DIRECTORY_ITERATOR_PREINDEX
	-------------------------------------
*/
class ANT_directory_iterator_preindex : public ANT_directory_iterator
{
friend class ANT_directory_iterator_preindex_internals;
private:
	ANT_producer_consumer <ANT_directory_iterator_object> *store;
	ANT_directory_iterator *source;
	long threads;
	ANT_critical_section mutex;
	ANT_memory_index *final_index;
	ANT_index_document index_document;
	ANT_indexer_param_block *param_block;

private:
	void work_one(ANT_directory_iterator_object *object, ANT_directory_iterator_preindex_internals *internals);
	void work(ANT_directory_iterator_preindex_internals *internals);
	static void *bootstrap(void *param);

public:
	ANT_directory_iterator_preindex(ANT_directory_iterator *source, ANT_indexer_param_block *param_block, ANT_index_document index_document_method, ANT_memory_index *final_index, long threads, long get_file);
	~ANT_directory_iterator_preindex();

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;



#endif /* DIRECTORY_ITERATOR_PREINDEX_H_ */
