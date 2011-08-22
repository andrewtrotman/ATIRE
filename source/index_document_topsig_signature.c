/*
	INDEX_DOCUMENT_TOPSIG_SIGNATURE.C
	---------------------------------
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <new>
#include "hash_table.h"
#include "index_document_topsig_signature.h"
#include "index_document_topsig.h"
#include "string_pair.h"
#include "maths.h"
#include "mersenne_twister.h"

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE()
	--------------------------------------------------------------------------
*/
ANT_index_document_topsig_signature::ANT_index_document_topsig_signature(long width, double density)
{
this->width = width;
this->density = density;

vector = new (std::nothrow) double[width];
rewind();
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::~ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE()
	---------------------------------------------------------------------------
*/
ANT_index_document_topsig_signature::~ANT_index_document_topsig_signature()
{
delete [] vector;
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::REWIND()
	---------------------------------------------
*/
void ANT_index_document_topsig_signature::rewind(void)
{
memset(vector, 0, sizeof(*vector) * width);
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ADD_TERM()
	-----------------------------------------------
*/
double ANT_index_document_topsig_signature::add_term(ANT_index_document_topsig *globalstats, char *term, long long term_frequency, long long document_length, long long collection_length_in_terms)
{
ANT_string_pair pair(term);

return add_term(globalstats, &pair, term_frequency, document_length, collection_length_in_terms);
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ADD_TERM()
	-----------------------------------------------
*/
double ANT_index_document_topsig_signature::add_term(ANT_index_document_topsig *globalstats, ANT_string_pair *term, long long term_frequency, long long document_length, long long collection_length_in_terms)
{
ANT_index_document_global_stats *collection_stats;
unsigned long long sign, cf, seed;
long bit;
double term_weight;
long num_positive;

if (ANT_isdigit(*term->string()))
	return 0.0;
if (ANT_isupper(*term->string()))
	return 0.0;

num_positive = (long)(width * (density / 200.0));

seed = ANT_random_hash_64(term->string(), term->length());

if ((collection_stats = globalstats->find(term)) == NULL)
	cf = term_frequency;			// use the term frequency as an estimate of collection frequency if the term is not in the global stats
else
	cf = collection_stats->collection_frequency;

/*
	Term weight is log(TF.ICF) but use TF/CL if cf=0 (so that we can use estimated of cf from a different collection)
*/
term_weight = log(((double)term_frequency / (double)document_length) * ((double)collection_length_in_terms / (double)cf));

if (term_weight < M_E)
	return 0.0;

for (bit = 0; bit < num_positive; bit++)
	{
	sign = ANT_rand_xorshift64(&seed) % width;
	vector[sign] += term_weight;

	sign = ANT_rand_xorshift64(&seed) % width;
	vector[sign] -= term_weight;
	}

return term_weight;
}
