/*
	INDEX_DOCUMENT_TOPSIG.H
	-----------------------
*/
#ifndef INDEX_DOCUMENT_TOPSIG_H_
#define INDEX_DOCUMENT_TOPSIG_H_

#include "index_document.h"

class ANT_memory_index;

/*
	class ANT_INDEX_DOCUMENT_TOPSIG
	-------------------------------
*/
class ANT_index_document_topsig : public ANT_index_document
{
private:
	long width;				// with of the signature (in bits)
	double density;			// number of +ve values in the signature as a percent value (0..100) (also the percent of -ve too, i.e. density * 2 = +ve + -ve)
	char *stats_file;		// name of the file that contains the cf statistics.  This file can be generated using ant_dictionary

	ANT_memory_index *global_stats;		// used to store the global stats once read from disk

public:
	ANT_index_document_topsig(long width, double density, char *global_stats_file);
	virtual ~ANT_index_document_topsig();
	
	virtual long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file);
} ;

#endif /* INDEX_DOCUMENT_TOPSIG_H_ */
