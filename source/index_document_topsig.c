/*
	INDEX_DOCUMENT_TOPSIG.C
	-----------------------
	This indexer inplements the TopSig indexing scheme as published by Shlomo Geva in CIKM 2011.
	As the paper has not yet appeared in print a full reference canot be given.
*/
#include "hash_table.h"
#include "disk.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "stem.h"
#include "mersenne_twister.h"
#include "numbers.h"
#include "index_document_topsig.h"
#include "maths.h"

/*
	ANT_INDEX_DOCUMENT_TOPSIG::HASH()
	---------------------------------
*/
inline long ANT_index_document_topsig::hash(ANT_string_pair *string)
{
return ANT_hash_24(string);
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::ANT_INDEX_DOCUMENT_TOPSIG()
	------------------------------------------------------
*/
ANT_index_document_topsig::ANT_index_document_topsig(long width, double density, char *global_stats_file)
{
long long unique_terms;
long file_read_error = true;
char *file, **line, **current, *space;
long long cf, uniq_terms;

this->width = width;
this->density = density;
this->stats_file = global_stats_file;
memset(hash_table, 0, sizeof(hash_table));

uniq_terms = collection_length_in_terms = 0;
if ((file = ANT_disk::read_entire_file(global_stats_file)) != NULL)
	if ((line = ANT_disk::buffer_to_list(file, &unique_terms)) != NULL)
		{
		file_read_error = false;
		for (current = line; *current != NULL; current++)
			if ((space = strchr(*current, ' ')) != NULL)
				{
				*space++ = '\0';
				while (!ANT_isdigit(*space) && *space != '\0')
					space++;
				cf = ANT_atoi64(space);
//				printf("Found:[%s]:%lld\n", *current, cf);
				if (**current != '~')
					{
					add(*current, cf);
					uniq_terms++;
					collection_length_in_terms += cf;
					}
				}
		}

printf("Topsig:Found %lld unique terms in collection of length %lld tokens\n", uniq_terms, collection_length_in_terms);

if (file_read_error)
	puts("Warning: TopSig cannot fread the frequencies file, defaulting to cf/l = 1");
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::~ANT_INDEX_DOCUMENT_TOPSIG()
	-------------------------------------------------------
*/
ANT_index_document_topsig::~ANT_index_document_topsig()
{
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::FIND_NODE()
	--------------------------------------
*/
ANT_index_document_global_stats *ANT_index_document_topsig::find_node(ANT_index_document_global_stats *root, ANT_string_pair *string)
{
long cmp;

if (root == NULL)
	return NULL;

while ((cmp = root->term.strcmp(string)) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			return NULL;
		else
			root = root->left;
	else
		if (root->right == NULL)
			return NULL;
		else
			root = root->right;
	}

return root;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::FIND_ADD_NODE()
	------------------------------------------
*/
ANT_index_document_global_stats *ANT_index_document_topsig::find_add_node(ANT_index_document_global_stats *root, ANT_string_pair *string)
{
long cmp;

while ((cmp = root->term.strcmp(string)) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			return root->left = new (NULL) ANT_index_document_global_stats(string);
		else
			root = root->left;
	else
		if (root->right == NULL)
			return root->right = new (NULL) ANT_index_document_global_stats(string);
		else
			root = root->right;
	}
return root;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::ADD()
	--------------------------------
*/
ANT_index_document_global_stats *ANT_index_document_topsig::add(char *string, long long collection_frequency)
{
ANT_index_document_global_stats *answer;
ANT_string_pair string_as_pair(string);
long hash_value = hash(&string_as_pair);

if (hash_table[hash_value] == NULL)
	answer = hash_table[hash_value] = new (NULL) ANT_index_document_global_stats(&string_as_pair);
else
	answer = find_add_node(hash_table[hash_value], &string_as_pair);

answer->collection_frequency = collection_frequency;

return answer;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::INDEX_DOCUMENT()
	-------------------------------------------
*/
long ANT_index_document_topsig::index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file)
{
ANT_index_document_global_stats *collection_stats;
unsigned long long negative_threshold, positive_threshold;
ANT_memory_indexer_node **term_list, **current;
ANT_memory_index_one *document_indexer;
long length, bit, hash_table_position;
long long cf;
uint64_t hash, sign;
double *document_vector, term_weight;
ANT_string_pair as_string;
int seed;

/*
	Set up the proportion of the number range that is +ve and -ve
*/
#ifdef ANT_TOPSIG_USES_TWISTER
	negative_threshold = (unsigned long long)(((unsigned long long)0xffffffffffffffff / 100) * density);
#else
	negative_threshold = (unsigned long long)(((ANT_RAND_MAX / 100) * density));
#endif
positive_threshold = negative_threshold + negative_threshold;

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
#ifdef ANT_TOPSIG_USES_TWISTER
	init_genrand64(hash);
#elae
	seed = (int)hash;
#endif
	hash_table_position = this->hash(&((*current)->string));
	cf = 1;
	if (hash_table[hash_table_position] != NULL)
		if ((collection_stats = find_node(hash_table[hash_table_position], &((*current)->string))) != NULL)
			cf = collection_stats->collection_frequency;
	/*
		Term weight is log(TF.ICF) but use 1/CL if cf=0 (so that we can use estimated of cf from a differnt collection)
	*/
	term_weight = log(((double)(*current)->term_frequency / (double)length) * ((double)collection_length_in_terms / (double)cf));

	for (bit = 0; bit < width; bit++)
		{
#ifdef ANT_TOPSIG_USES_TWISTER
		sign = genrand64_int64();
#else
		sign = ANT_rand(&seed);
#endif
		if (sign < negative_threshold)
			document_vector[bit] += -term_weight;
		else if (sign < positive_threshold)
			document_vector[bit] += term_weight;
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

