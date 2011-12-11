/*
	INDEX_DOCUMENT_TOPSIG.H
	-----------------------
*/
#ifndef INDEX_DOCUMENT_TOPSIG_H_
#define INDEX_DOCUMENT_TOPSIG_H_

#include "index_document.h"
#include "index_document_global_stats.h"

class ANT_memory_index;
class ANT_index_document_topsig_signature;
class ANT_memory_index_one;

/*
	class ANT_INDEX_DOCUMENT_TOPSIG
	-------------------------------
*/
class ANT_index_document_topsig : public ANT_index_document
{
friend class ANT_index_document_topsig_signature;
private:
	static const long HASH_TABLE_SIZE = 0x1000000;

private:
	long width;				// with of the signature (in bits)
	double density;			// number of +ve values in the signature as a percent value (0..100) (also the percent of -ve too, i.e. density * 2 = +ve + -ve)
	char *stats_file;		// name of the file that contains the cf statistics.  This file can be generated using ant_dictionary
	char *term_source;		// the contents of the stats file (read at startup and deleted on shutdown)

	ANT_index_document_global_stats *hash_table[HASH_TABLE_SIZE];		// hold the global collection frequency counts
	long long collection_length_in_terms;								// as is says

private:
	long hash(ANT_string_pair *string);

protected:
	ANT_index_document_global_stats *find_node(ANT_index_document_global_stats *root, ANT_string_pair *string);
	ANT_index_document_global_stats *find_add_node(ANT_index_document_global_stats *root, ANT_string_pair *string);
	ANT_index_document_global_stats *add(char *string, long long collection_frequency, long long document_frequency);
	ANT_index_document_global_stats *find(ANT_string_pair *string);

public:
	ANT_index_document_topsig(long stop_mode, long width, double density, char *global_stats_file);
	virtual ~ANT_index_document_topsig();
	
	virtual long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc_id, unsigned char *file);
	long long get_collection_length(void) { return collection_length_in_terms; }
} ;

#endif /* INDEX_DOCUMENT_TOPSIG_H_ */
