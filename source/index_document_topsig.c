/*
	INDEX_DOCUMENT_TOPSIG.C
	-----------------------
	This indexer inplements the TopSig indexing scheme as published by Shlomo Geva in CIKM 2011.
	As the paper has not yet appeared in print a full reference canot be given.
*/
#include <new>
#include "hash_table.h"
#include "disk.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "stem.h"
#include "numbers.h"
#include "maths.h"
#include "index_document_topsig.h"
#include "index_document_topsig_signature.h"

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
ANT_index_document_topsig::ANT_index_document_topsig(long stop_mode, long width, double density, char *global_stats_file) : ANT_index_document(stop_mode)
{
long long unique_terms;
long file_read_error = true;
char *file, **current, *space, **line = NULL;
long long cf, df, uniq_terms;

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

				while (ANT_isdigit(*space) && *space != '\0')
					space++;
				while (!ANT_isdigit(*space) && *space != '\0')
					space++;
				df = ANT_atoi64(space);

//				printf("Found:[%s]:%lld %lld\n", *current, cf, df);

				if (**current != '~')
					{
					add(*current, cf, df);
					uniq_terms++;
					collection_length_in_terms += cf;
					}
				}
		}

printf("Topsig:Found %lld unique terms in collection of length %lld tokens\n", uniq_terms, collection_length_in_terms);

if (file_read_error)
	puts("Warning: TopSig cannot fread the frequencies file, defaulting to cf/l = 1");

term_source = file;
delete [] line;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::~ANT_INDEX_DOCUMENT_TOPSIG()
	-------------------------------------------------------
*/
ANT_index_document_topsig::~ANT_index_document_topsig()
{
delete [] term_source;
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
	ANT_INDEX_DOCUMENT_TOPSIG::FIND()
	---------------------------------
*/
ANT_index_document_global_stats *ANT_index_document_topsig::find(ANT_string_pair *string)
{
long hash_table_position;

hash_table_position = hash(string);

if (hash_table[hash_table_position] == NULL)
	return NULL;
else
	return find_node(hash_table[hash_table_position], string);
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
ANT_index_document_global_stats *ANT_index_document_topsig::add(char *string, long long collection_frequency, long long document_frequency)
{
ANT_index_document_global_stats *answer;
ANT_string_pair string_as_pair(string);
long hash_value = hash(&string_as_pair);

if (hash_table[hash_value] == NULL)
	answer = hash_table[hash_value] = new (NULL) ANT_index_document_global_stats(&string_as_pair);
else
	answer = find_add_node(hash_table[hash_value], &string_as_pair);

answer->collection_frequency = collection_frequency;
answer->document_frequency = document_frequency;

return answer;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG::INDEX_DOCUMENT()
	-------------------------------------------
*/
long ANT_index_document_topsig::index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc_id, unsigned char *file)
{
unsigned long long seed = 0;
ANT_memory_indexer_node **term_list, **current;
long length, bit;
double *vector;
ANT_string_pair as_string;
ANT_memory_index_one *document_indexer;					// the object that generates the initial ANT index
ANT_index_document_topsig_signature *signature;			// the current signature we're working on

/*
	allocate all the necessary memory
*/
signature = new (std::nothrow) ANT_index_document_topsig_signature(width, density, stopword_mode);
document_indexer = new (std::nothrow) ANT_memory_index_one(new ANT_memory(1024 * 1024));

/*
	First pass index and get the list of terms and term counts
*/
length = ANT_index_document::index_document(document_indexer, stemmer, segmentation, readability, doc_id, file);
term_list = document_indexer->get_term_list();

/*
	Now walk the term list generating the signatures
*/
length = 0;
for (current = term_list; *current != NULL; current++)
	if ((seed = signature->add_term(this, &((*current)->string), (*current)->term_frequency, length, collection_length_in_terms)) != 0)
		length++;

/*
	Walk the bit string converting +ve and 0 into 1s (i.e. postings in a postings list)
*/
vector = signature->get_vector();
for (bit = 0; bit < width; bit++)
	{
	if (vector[bit] > 0)		// positive values and get encoded as a 1
		{
		ANT_atosp(&as_string, bit);
		indexer->add_term(&as_string, doc_id);
		}
	else if (vector[bit] == 0)	// zero values are encoded (systematically) randomly as 0 or 1
		{
		if (ANT_rand_xorshift64(&seed) & 1)
			{
			ANT_atosp(&as_string, bit);
			indexer->add_term(&as_string, doc_id);
			}
		}
	}

/*
	Set the document's length in terms (needed for tie breaks on equal hamming distance and for search engie initialisation)
*/
indexer->set_document_length(doc_id, length);

/*
  clean up and return the length of the document
*/
delete [] term_list;
delete document_indexer;
delete signature;

return length;
}

