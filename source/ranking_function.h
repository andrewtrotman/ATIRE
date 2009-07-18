/*
	ANT_RANKING_FUNCTION.H
	----------------------
*/
#ifndef __ANT_RANKING_FUNCTION_H__
#define __ANT_RANKING_FUNCTION_H__

#include "compress.h"

class ANT_search_engine;
class ANT_search_engine_accumulator;
class ANT_search_engine_btree_leaf;
class ANT_search_engine_stats;

/*
	class ANT_RANKING_FUNCTION
	--------------------------
*/
class ANT_ranking_function
{
protected:
	ANT_search_engine *engine;
	long long documents;
	double mean_document_length;
	long *document_lengths;
	ANT_compressable_integer *decompress_buffer;
	ANT_search_engine_stats *stats;

protected:
	void tf_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *destination, long *stem_buffer);
	void compute_term_details(ANT_search_engine_btree_leaf *term_details, long *tf_array);

public:
	ANT_ranking_function(ANT_search_engine *engine);
	virtual ~ANT_ranking_function() {}

	/*
		You must override this function if you're going to add a ranking function
	*/
	virtual void relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering) = 0;
	
	/*
		If you also override this function then you can rank directly from the tf array,
		but be warned, the search engine does not at this time know the value of term_details->document_frequency
		so you'll have to compute that using compute_term_details() requires a scan of the tf array.  Also note
		that the number of floating point computations you'll need to do could be sufficiently large that it takes
		less time to convert into a postings list and call relevance_rank_top_k (which is the default behaviour).
	*/
	virtual void relevance_rank_tf(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, long *tf_array);
} ;

#endif __ANT_RANKING_FUNCTION_H__
