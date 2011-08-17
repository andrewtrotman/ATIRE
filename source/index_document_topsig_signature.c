/*
	INDEX_DOCUMENT_TOPSIG_SIGNATURE.C
	---------------------------------
*/
#include "hash_table.h"
#include "new"
#include "index_document_topsig_signature.h"
#include "index_document_topsig.h"
#include "string_pair.h"
#include "maths.h"
#include "mersenne_twister.h"

//////#define ANT_TOPSIG_USES_TWISTER 1

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE()
	--------------------------------------------------------------------------
*/
ANT_index_document_topsig_signature::ANT_index_document_topsig_signature(long width, double density)
{
this->width = width;
this->density = density;

vector = new (std::nothrow) double[width];

#ifdef ANT_TOPSIG_USES_TWISTER
	negative_threshold = (unsigned long long)(((unsigned long long)0xffffffffffffffff / 100) * density);
#else
	negative_threshold = (unsigned long long)(((ANT_RAND_MAX / 100) * density));
#endif
positive_threshold = negative_threshold + negative_threshold;

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

#ifdef NEVER
	/*
		ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ADD_TERM()
		-----------------------------------------------
	*/
	double ANT_index_document_topsig_signature::add_term(ANT_index_document_topsig *globalstats, ANT_string_pair *term, long long term_frequency, long long document_length, long long collection_length_in_terms)
	{
	ANT_index_document_global_stats *collection_stats;
	unsigned long long hash, sign, cf;
	long bit;
	double term_weight;

	if (ANT_isdigit(*term->string()))
		return 0.0;

	hash = ANT_random_hash_64(term->string(), term->length());
	#ifdef ANT_TOPSIG_USES_TWISTER
		init_genrand64(hash);
	#else
		int seed = (int)hash;
	#endif
	if ((collection_stats = globalstats->find(term)) == NULL)
		cf = term_frequency;			// use the term frequency as an estimate of collection frequency if the term is not in the global stats
	else
		cf = collection_stats->collection_frequency;
	/*
		Term weight is log(TF.ICF) but use TF/CL if cf=0 (so that we can use estimated of cf from a different collection)
	*/
	term_weight = log(((double)term_frequency / (double)document_length) * ((double)collection_length_in_terms / (double)cf));

	for (bit = 0; bit < width; bit++)
		{
		#ifdef ANT_TOPSIG_USES_TWISTER
			sign = genrand64_int64();
		#else
			sign = ANT_rand_xorshift64(&seed);
		#endif
		if (sign < negative_threshold)
			vector[bit] -= term_weight;
		else if (sign < positive_threshold)
			vector[bit] += term_weight;
		}

	return term_weight;
	}
#endif

/*
	ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE::ADD_TERM()
	-----------------------------------------------
*/
double ANT_index_document_topsig_signature::add_term(ANT_index_document_topsig *globalstats, ANT_string_pair *term, long long term_frequency, long long document_length, long long collection_length_in_terms)
{
ANT_index_document_global_stats *collection_stats;
unsigned long long hash, sign, cf;
long bit;
double term_weight;
long num_positive;

if (ANT_isdigit(*term->string()))
	return 0.0;

num_positive = (long)(width * (density / 200.0));

hash = ANT_random_hash_64(term->string(), term->length());
#ifdef ANT_TOPSIG_USES_TWISTER
	init_genrand64(hash);
	puts("TWISTER");
#else
	unsigned long long seed = hash;
#endif
if ((collection_stats = globalstats->find(term)) == NULL)
	cf = term_frequency;			// use the term frequency as an estimate of collection frequency if the term is not in the global stats
else
	cf = collection_stats->collection_frequency;
/*
	Term weight is log(TF.ICF) but use TF/CL if cf=0 (so that we can use estimated of cf from a different collection)
*/
term_weight = log(((double)term_frequency / (double)document_length) * ((double)collection_length_in_terms / (double)cf));

for (bit = 0; bit < num_positive; bit++)
	{
	#ifdef ANT_TOPSIG_USES_TWISTER
		sign = genrand64_int64();
	#else
		sign = ANT_rand_xorshift64(&seed);
	#endif
	sign %= width;
	vector[sign] += term_weight;


	#ifdef ANT_TOPSIG_USES_TWISTER
		sign = genrand64_int64();
	#else
		sign = ANT_rand_xorshift64(&seed);
	#endif
	sign %= width;
	vector[sign] -= term_weight;
	}

return term_weight;
}
