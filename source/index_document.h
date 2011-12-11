/*
	INDEX_DOCUMENT.H
	----------------
*/
#ifndef INDEX_DOCUMENT_H_
#define INDEX_DOCUMENT_H_

class ANT_memory_indexer;
class ANT_stem;
class ANT_readability_factory;
class ANT_directory_iterator_object;

/*
	class ANT_INDEX_DOCUMENT
	------------------------
*/
class ANT_index_document
{
protected:
	long stopword_mode;

public:
	ANT_index_document(long stop_mode = 0) { stopword_mode = stop_mode; }

	virtual long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc_id, unsigned char *file);
	virtual long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc_id, char *file) { return index_document(indexer, stemmer, segmentation, readability, doc_id, (unsigned char *)file); }
} ;

#endif /* INDEX_DOCUMENT_H_ */

