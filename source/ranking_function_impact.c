/*
	RANKING_FUNCTION_IMPACT.C
	-------------------------
*/
#include <math.h>
#include "ranking_function_impact.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_IMPACT::RELEVANCE_RANK_TOP_K()
	---------------------------------------------------
*/
void ANT_ranking_function_impact::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid, tf;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;		// do the conversion from integet to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator[docid].add_rsv(tf);
		}
	current++;
	}
}

