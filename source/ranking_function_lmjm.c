/*
	RANKING_FUNCTION_LMJM.C
	-----------------------
*/

#include <math.h>
#include "ranking_function_lmjm.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_LMJM::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
	Language Models with Jelinek-Mercer smoothing
*/
void ANT_ranking_function_lmjm::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid;
double tf, rsv;
double one_minus_lambda, idf;
ANT_compressable_integer *current, *end;

/*
                 1 - lambda   tf(dt)   len(c)
   rsv = log(1 + ---------- * ------ * ------
                   lambda     len(d)    cf(t)
*/
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
one_minus_lambda = (1.0 - lambda) / lambda;
idf =  (double)collection_length_in_terms / (double)term_details->collection_frequency;
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		rsv = one_minus_lambda * (tf / (double)document_lengths[docid]) * idf;
		accumulator[docid].add_rsv(rsv);
		}
	current++;		// skip over the zero
	}
}
