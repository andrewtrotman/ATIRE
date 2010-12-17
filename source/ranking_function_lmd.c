/*
	RANKING_FUNCTION_LMD.C
	----------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_lmd.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_LMD::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmd::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long docid;
double tf, idf, n;
double left_hand_side, rsv;
ANT_compressable_integer *current, *end;

/*
                 tf(td)   len(c)              len(d)
   rsv = log(1 + ------ * ------) - n log(1 + ------)
                    u     cf(t)                 u

	where  len(c) is the length of the collection (in terms), len(d) is the length of the document
	tf(td) is the term frequency of the term and cf(t) is the collection_frequency of the term
	and n is the length of the querty in terms.
*/
n = 3.0;						// this is a hack and should be the length of the query
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
idf = ((double)collection_length_in_terms / (double)term_details->collection_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	left_hand_side = log (1.0 + (prescalar * tf / u) * idf);
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		rsv = left_hand_side - n * log(1.0 + ((double)document_lengths[docid] / u));
		accumulator->add_rsv(docid, postscalar * rsv);
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_LMD::RANK()
	--------------------------------
*/
double ANT_ranking_function_lmd::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
double tf, idf, n, left_hand_side;

n = 3.0;						// this is a hack and should be the length of the query
tf = term_frequency;
idf = ((double)collection_length_in_terms / (double)collection_frequency);
left_hand_side = log (1.0 + (tf / u) * idf);

return left_hand_side - n * log(1.0 + ((double)document_lengths[docid] / u));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
