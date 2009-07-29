/*
	RANKING_FUNCTION_BOSE_EINSTEIN.C
	--------------------------------
*/
#include <math.h>
#include "ranking_function_bose_einstein.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------------
	Ranking function GL2 from:
	G. Amati and C.J. van Rijsbergen (2002), Probabilistic Models of Information Retrieval Based on 
	Measuring the Divergence from Randomness, Transactions on Information Systems 20(4):357-389.
*/
void ANT_ranking_function_bose_einstein::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid;
double tf, rsv, left, right, tf_prime;
ANT_compressable_integer *current, *end;

/*
	      log(1 + cf(t) / N) + tf'(td) * log (1 + N / cf(t))
	rsv = --------------------------------------------------
	                        tf'(td) + 1

	where
		tf'(td) = tf(td) * log(1 + av_len_d / len(d))


	This implementation has:
		Inf =according to the Geometric estimation of the Bose Einstein Model of Randomness
		First Normalisation according to Laplace's Law of Succession
		Second (length) normalisation according to H2
	and is therefore GL2
		
*/
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);

left = log(1.0 + (double)term_details->collection_frequency / (double)documents);
right = log(1.0 + (double)documents / (double)term_details->collection_frequency);

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		tf_prime = tf * log(1.0 + mean_document_length / (double)document_lengths[docid]);
		rsv = (left + tf_prime * right) / (tf_prime + 1.0);
		accumulator[docid].add_rsv(rsv);
		}
	current++;		// skip over the zero
	}
}
