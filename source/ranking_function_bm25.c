/*
	RANKING_FUNCTION_BM25.C
	-----------------------
*/
#include <math.h>
#include "ranking_function_bm25.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BM25::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
*/
void ANT_ranking_function_BM25::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering)
{
const double k1 = this->k1;
const double b = this->b;
const double k1_plus_1 = k1 + 1.0;
const double one_minus_b = 1.0 - b;
long docid;
double top_row, tf, idf;
ANT_compressable_integer *current, *end;

/*
	          N
	IDF = log -
	          n

	This variant of IDF is better than log((N - n + 0.5) / (n + 0.5)) on the 70 INEX 2008 Wikipedia topics
*/
idf = log((double)(documents) / (double)term_details->document_frequency);

/*
	               tf(td) * (k1 + 1)
	rsv = ----------------------------------- * IDF
	                                  len(d)
	      tf(td) + k1 * (1 - b + b * --------)
                                    av_len_d

	In this implementation we ignore k3 and the number of times the term occurs in the query.
*/
current = impact_ordering;
end = impact_ordering + term_details->impacted_length;
while (current < end)
	{
	tf = *current++;
	top_row = tf * k1_plus_1;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator[docid].add_rsv(idf * (top_row / (tf + k1 * (one_minus_b + b * (document_lengths[docid] / mean_document_length)))));
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_BM25::RELEVANCE_RANK_TF()
	----------------------------------------------
	This is the variant that gets called for stemming
*/
void ANT_ranking_function_BM25::relevance_rank_tf(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, long *tf_array)
{
const double k1 = this->k1;
const double b = this->b;
const double k1_plus_1 = k1 + 1.0;
const double one_minus_b = 1.0 - b;
double tf, idf, top_row;
long *current, *end;
long docid;

compute_term_details(term_details, tf_array);	// get document_frequency;

idf = log((double)(documents) / (double)term_details->document_frequency);

end = tf_array + documents;
for (current = tf_array; current < end; current++)
	if (*current != 0)
		{
		tf = *current >= 0x100 ? 0xFF : *current;
		docid = current - tf_array;
		top_row = tf * k1_plus_1;

		accumulator[docid].add_rsv(idf * (top_row / (tf + k1 * (one_minus_b + b * (document_lengths[docid] / mean_document_length)))));
		}
}
