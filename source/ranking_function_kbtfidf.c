/*
	RANKING_FUNCTION_KBTFIDF.C
	--------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_kbtfidf.h"
#include "compress.h"
#include "search_engine_btree_leaf.h"

/*
	ANT_RANKING_FUNCTION_KBTFIDF::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_kbtfidf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
double tf, idf, score;
long docid;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
idf = log((double)documents / (double)term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = prescalar * *current++;
	score = postscalar * (log(k * tf - b) * idf * idf) / 100.0;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, score);
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_KBTFIDF::RANK()
	------------------------------------
*/
double ANT_ranking_function_kbtfidf::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
double idf, tf, rsv;

tf = (double)term_frequency;
idf = log((double)documents / (double)document_frequency);

rsv = log(k * tf - b) * idf * idf;
	
return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
