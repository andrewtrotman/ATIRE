/*
	ANT_RANKING_FUNCTION.H
	----------------------
*/
#ifndef ANT_RANKING_FUNCTION_H_
#define ANT_RANKING_FUNCTION_H_

#include "compress.h"
#include "search_engine_accumulator.h"
#include "search_engine_result.h"
#ifdef IMPACT_HEADER
#include "ranking_function_quantum_parameters.h"
#endif

#ifdef IMPACT_HEADER
#include "impact_header.h"
#endif

class ANT_search_engine;
class ANT_search_engine_btree_leaf;
class ANT_stats_search_engine;
class ANT_bitstring;

#ifdef USE_FLOATED_TF
typedef double ANT_weighted_tf;
#else
typedef long ANT_weighted_tf;
#endif

/*
	class ANT_RANKING_FUNCTION
	--------------------------
*/
class ANT_ranking_function
{
protected:
	ANT_search_engine *engine;
	double documents;
	long long documents_as_integer;
	long quantization;
	long long quantization_bits;
	double minimum_collection_rsv, maximum_collection_rsv;
	double collection_length_in_terms;
	long long collection_length_in_terms_as_integer;
	double mean_document_length;
	ANT_compressable_integer *document_lengths;
	ANT_compressable_integer *decompress_buffer;
	ANT_stats_search_engine *stats;
#ifdef IMPACT_HEADER
	ANT_impact_header *the_impact_header;
#endif

public:
#ifdef IMPACT_HEADER
	void tf_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *destination, ANT_impact_header *impact_header, ANT_weighted_tf *stem_buffer);
#else
	void tf_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *destination, ANT_weighted_tf *stem_buffer);
#endif
void compute_term_details(ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array);

public:
	/*
		This constructor is called from the search engine.
		We might want to quantize at search time, to find the best quantization bits, which we can then push to the index.
	*/
	ANT_ranking_function(ANT_search_engine *engine, long quantize, long long quantization_bits);

	/*
		This constructor is called for quantized impact ordering during indexing
	*/
	ANT_ranking_function(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits = 8);

	/*
		Nothing to destroy
	*/
	virtual ~ANT_ranking_function() {}

	/*
		You must override these functions if you're going to add a ranking function.
	*/
#ifdef IMPACT_HEADER
	virtual void relevance_rank_quantum(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
	virtual void relevance_rank_quantum(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point) { relevance_rank_quantum(accumulator, term_details, impact_header, impact_ordering, trim_point, 1.0, 1.0); };
	virtual void relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_paramters) = 0;
#endif

#ifdef IMPACT_HEADER
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar) = 0;
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar) = 0;
#endif
	virtual double rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency) = 0;

	/*
		If you override this one you can avoid a couple of multiplies when the prescalar and postscalar are both 1
	*/
#ifdef IMPACT_HEADER
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point) { relevance_rank_top_k(accumulators, term_details, impact_header, impact_ordering, trim_point, 1.0, 1.0); }
#else
	virtual void relevance_rank_top_k(ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point) { relevance_rank_top_k(accumulators, term_details, impact_ordering, trim_point, 1.0, 1.0); }
#endif
	/*
		You can override this function if you want fast boolean, otherwise it'll run through the postings list
		setting bits and then chain through to relevance_rank_top_k().  That is, default behaviour is to double
		pass the postings list but this can be overridden to touch the postings list only once (see BM25 for
		and example).
	*/
#ifdef IMPACT_HEADER
	virtual void relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#else
	virtual void relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar);
#endif

	/*
		If you override this one you can avoid a couple of multiplies when the prescalar and postscalar are both 1
	*/
#ifdef IMPACT_HEADER
	virtual void relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point) { relevance_rank_boolean(documents_touched, accumulators, term_details, impact_header, impact_ordering, trim_point, 1.0, 1.0); }
#else
	virtual void relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point) { relevance_rank_boolean(documents_touched, accumulators, term_details, impact_ordering, trim_point, 1.0, 1.0); }
#endif

	/*
		If you also override this function then you can rank directly from the tf array,
		but be warned, the search engine does not at this time know the value of term_details->document_frequency
		so you'll have to compute that using compute_term_details() requires a scan of the tf array.  Also note
		that the number of floating point computations you'll need to do could be sufficiently large that it takes
		less time to convert into a postings list and call relevance_rank_top_k (which is the default behaviour).

		Of new note is that the trim_point gets automaticly chained on to relevance_rank_top_k() and so once the
		postings list is generated from the tf array it is then (correctly) trimmed thus further reducing the
		computational cost of the search.
	*/
	virtual void relevance_rank_tf(ANT_bitstring *bitstring, ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array, long long trim_point, double prescalar, double postscalar);

	/*
		Functions used for quantised impact ordering.  We need to compute the range of values that will be
		computed from the ranking function before we quantize.  We also need a function that will take the
		postings list and quantize it.  The two default functions provided here call a virtual function
		rank() that returns the score for an individual document - yes, it is slow, but it is the default
		behaviour.  Anyone wanting to hack up a ranking function need only supply the one rank() function
		but anyone wanting efficient support needs to supply several funcitons including these two.
	*/
	virtual void get_max_min(double *maximum, double *minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned short *term_frequencies);
	virtual void quantize(double maximum, double minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned short *term_frequencies);
	double quantize(double rsv, double maximum, double minimum);
} ;

#endif  /* ANT_RANKING_FUNCTION_H_ */
