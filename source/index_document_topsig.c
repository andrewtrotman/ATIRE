/*
	INDEX_DOCUMENT_TOPSIG.C
	-----------------------
	This indexer inplements the TopSig indexing scheme as published by Shlomo Geva in CIKM 2011.
	As the paper has not yet appeared in print a full reference canot be given
*/
#include "hash_table.h"
#include "index_document.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "stem.h"
#include "readability_factory.h"
#include "directory_iterator_object.h"
#include "mersenne_twister.h"
#include "numbers.h"
#include "index_document_topsig.h"

/*
	ANT_INDEX_DOCUMENT_TOPSIG::INDEX_DOCUMENT_TOPSIG()
	--------------------------------------------------
*/
long ANT_index_document_topsig::index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file)
{
static const long width = 4096;
static const long density = 12;
static const long long negative_threshold = width / density;
static const long long positive_threshold = negative_threshold + (width / density);
ANT_memory_indexer_node **term_list, **current;
ANT_memory_index_one *document_indexer;
long length, bit;
uint64_t hash, sign;
double *document_vector, term_weight;
ANT_string_pair as_string;

/*
	Allocate space for the vector;
*/
document_vector = new double [width];
memset(document_vector, 0, sizeof(*document_vector) * width);

/*
	Index the document so as to get term counts
*/
document_indexer = new ANT_memory_index_one(new ANT_memory(1024 * 1024));
length = ANT_index_document::index_document(document_indexer, stemmer, segmentation, readability, doc, current_file);

/*
	Get the list of terms and the term counts
*/
term_list = document_indexer->get_term_list();

/*
	Now walk the term list generating the signatures
*/
for (current = term_list; *current != NULL; current++)
	{
//	printf("%*.*s %lld\n", (*current)->string.length(), (*current)->string.length(), (*current)->string.string(), (*current)->term_frequency);
	hash = ANT_random_hash_64((*current)->string.string(), (*current)->string.length());
	init_genrand64(hash);
	term_weight = (double)((*current)->term_frequency) / (double)length;			// TO DO: should be log(TF.ICF)
	for (bit = 0; bit < width; bit++)
		{
		sign = genrand64_int64();
		if (sign < negative_threshold)
			document_vector[bit] += -term_weight;
		else if (sign < positive_threshold)
			document_vector[bit] = term_weight;
		}
	}

/*
	Walk the bit string converting +ve and 0 into 1s (i.e. postings in a postings list)
*/
for (bit = 0; bit < width; bit++)
	if (document_vector[bit] >= 0)		// positive values and 0 get encoded as a 1, all other values as 0
		{
		if (ANT_atosp(&as_string, bit) != NULL)
			indexer->add_term(&as_string, doc);
		else
			exit(printf("TopSig does not support bitstrings as long as %ld bits\n", bit));
		}

/*
	Set the document's length in terms (needed for tie breaks on equal hamming distance and for search engie initialisation)
*/
indexer->set_document_length(doc, length);

/*
	And clean up
*/
delete [] document_vector;
delete document_indexer;
return length;
}

