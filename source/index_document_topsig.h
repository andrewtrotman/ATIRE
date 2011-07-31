/*
	INDEX_DOCUMENT_TOPSIG.H
	-----------------------
*/
#ifndef INDEX_DOCUMENT_TOPSIG_H_
#define INDEX_DOCUMENT_TOPSIG_H_

class ANT_memory_indexer;
class ANT_stem;
class ANT_readability_factory;
class ANT_directory_iterator_object;

long index_document_topsig(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file);

#endif /* INDEX_DOCUMENT_TOPSIG_H_ */
