/*
	INDEX_DOCUMENT_TOPSIG.H
	-----------------------
*/
#ifndef INDEX_DOCUMENT_TOPSIG_H_
#define INDEX_DOCUMENT_TOPSIG_H_

#include "index_document.h"

/*
	class ANT_INDEX_DOCUMENT_TOPSIG
	-------------------------------
*/
class ANT_index_document_topsig : public ANT_index_document
{
public:
	virtual long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file);
} ;

#endif /* INDEX_DOCUMENT_TOPSIG_H_ */
