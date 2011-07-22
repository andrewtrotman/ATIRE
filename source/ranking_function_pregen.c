/*
	RANKING_FUNCTION_PREGEN.C
	-----------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_pregen.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"
#include <limits>

/*
	ANT_RANKING_FUNCTION_PREGEN::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------------
*/
void ANT_ranking_function_pregen::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
(void) prescalar;
(void) postscalar;

long docid;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->set_rsv(docid, ascending ? std::numeric_limits<ANT_search_engine_accumulator::ANT_accumulator_t>::max() - pregen->scores[docid] : pregen->scores[docid]);
		}
	current++;
	}
}

/*
	ANT_RANKING_FUNCTION_PREGEN::RANK()
	---------------------------------------
*/
double ANT_ranking_function_pregen::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
return (double)1;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
