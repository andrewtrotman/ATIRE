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
unsigned long long ANT_index_document_topsig_signature::add_term(ANT_index_document_topsig *globalstats, char *term, long long term_frequency, long long document_length, long long collection_length_in_terms)
{
ANT_string_pair pair(term);

return add_term(globalstats, &pair, term_frequency, document_length, collection_length_in_terms);
}

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ADD_TERM()
	-----------------------------------------------
*/
unsigned long long ANT_index_document_topsig_signature::add_term(ANT_index_document_topsig *globalstats, ANT_string_pair *term, long long term_frequency, long long document_length, long long collection_length_in_terms)
{
ANT_index_document_global_stats *collection_stats;
unsigned long long sign, cf, seed;
double term_weight;
long bit, num_positive;

/*
	Hash the term to seed the random number generator
*/
seed = ANT_random_hash_64(term->string(), term->length());

/*
	Check that we should index the term (no numbers, and no XML tags).
	If we ditch the word then return the hash (because we might need the seed later)
*/
if (ANT_isdigit(*term->string()))
	return seed;
if (ANT_isupper(*term->string()))
	return seed;

/*
	Look up the collection frequency of the term.  It might not be there because
	the term list might have come from a different collection not containing this
	term.  In that case default to cf=tf
*/
if ((collection_stats = globalstats->find(term)) == NULL)
	cf = term_frequency;			// use the term frequency as an estimate of collection frequency if the term is not in the global stats
else
	cf = collection_stats->collection_frequency;

/*
	the term's weight is log(TF.ICF)
*/
term_weight = log(((double)term_frequency / (double)document_length) * ((double)collection_length_in_terms / (double)cf));

//printf("%*.*s: (%lld/%lld)*(%lld/%lld)=%f\n", term->length(), term->length(), term->string(), (long long)term_frequency, (long long)document_length, (long long)collection_length_in_terms, (long long)cf, term_weight);

/*
	Stop word removal
*/
if (term_weight < M_E)
	return seed;

/*
	how many random +ve and -ve numbers do we need?
*/
num_positive = (long)(width * (density / 200.0));			// divide by 200 because num_positive = 1/2 num_random, and its a percent

/*
	add the term's random vector to the document's random vector
*/
for (bit = 0; bit < num_positive; bit++)
	{
	sign = ANT_rand_xorshift64(&seed) % width;
	vector[(size_t)sign] += term_weight;

	sign = ANT_rand_xorshift64(&seed) % width;
	vector[(size_t)sign] -= term_weight;
	}

/*
	return the next seed for the random number generator
*/
return seed;
}
