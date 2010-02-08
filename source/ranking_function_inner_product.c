/*
	RANKING_FUNCTION_INNER_PRODUCT.C
	--------------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_inner_product.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_INNER_PRODUCT::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------------
*/
void ANT_ranking_function_inner_product::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid;
double tf, idf;
ANT_compressable_integer *current, *end;

/*
	          N
	IDF = log -
	          n
*/

idf = log((double)documents / (double)term_details->document_frequency);

/*
	rsv = (tf_in_d * IDF) * (tf_in_q * IDF)

	where tf_in_d is the tf in the document and tf_in_q is 1 and so we get tf * IDF * IDF
*/
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, (tf * idf * idf) / 100.0);  // TF.IDF scores blow-out the integer accumulators and so we shift the decimal place back a bit
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_INNER_PRODUCT::RANK()
	------------------------------------------
*/
double ANT_ranking_function_inner_product::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
double idf, tf, rsv;

tf = (double)term_frequency;
idf = log((double)documents / (double)document_frequency);
rsv = tf * idf * idf;

return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
