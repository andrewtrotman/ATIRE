/*
	RANKING_FUNCTION_TERM_COUNT.C
	-----------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_term_count.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_TERM_COUNT::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------------
*/
void ANT_ranking_function_term_count::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid, tf;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, (long)1);
		}
	current++;
	}
}

/*
	ANT_RANKING_FUNCTION_TERM_COUNT::RANK()
	---------------------------------------
*/
double ANT_ranking_function_term_count::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
return (double)1;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
