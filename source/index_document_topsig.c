/*
	INDEX_DOCUMENT_TOPSIG.C
	-----------------------
	This indexer inplements the TopSig indexing scheme as published by Shlomo Geva in CIKM 2011.
	As the paper has not yet appeared in print a full reference canot be given
*/
#include "hash_table.h"
#include "disk.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "stem.h"
#include "mersenne_twister.h"
#include "numbers.h"
#include "index_document_topsig.h"

/*
	ANT_INDEX_DOCUMENT_TOPSIG::ANT_INDEX_DOCUMENT_TOPSIG()
	------------------------------------------------------
*/
ANT_index_document_topsig::ANT_index_document_topsig(long width, double density, char *global_stats_file)
{
long long unique_terms;
long cf, file_read_error = true;
char *file, **line, **current, *space;

this->width = width;
this->density = density;
this->stats_file = global_stats_file;
global_stats = new ANT_memory_index(NULL);


if ((file = ANT_disk::read_entire_file(global_stats_file)) != NULL)
	if ((line = ANT_disk::buffer_to_list(file, &unique_terms)) != NULL)
		{
		file_read_error = false;
		for (current = line; *current != NULL; current++)
			if (**current != '~')
				{
				if ((space = strchr(*current, ' ')) != NULL)
					{
					cf = atol(space);
					*space = '\0';
					printf("Found:[%s]:%ld\n", *current, cf);
					}
				}
			else
				puts(*current);
		}

if (file_read_error)
	puts("Warning: TopSig cannot fread the frequencies file, defaulting to cf/l = 1");
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::~ANT_INDEX_DOCUMENT_TOPSIG()
	-------------------------------------------------------
*/
ANT_index_document_topsig::~ANT_index_document_topsig()
{
delete global_stats;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::INDEX_DOCUMENT()
	-------------------------------------------
*/
long ANT_index_document_topsig::index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file)
{
unsigned long long negative_threshold = (long long)(width * (density / 100.0));
unsigned long long positive_threshold = negative_threshold + negative_threshold;
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

