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

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_TERM_COUNT::RELEVANCE_RANK_ONE_QUANTUM()
	-------------------------------------------------------------
*/
void ANT_ranking_function_term_count::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
ANT_compressable_integer *current;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	quantum_parameters->accumulator->add_rsv(docid, (long)1);
	}
}

/*
	ANT_RANKING_FUNCTION_TERM_COUNT::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------------
*/
void ANT_ranking_function_term_count::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
ANT_compressable_integer *current, *end;

impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		accumulator->add_rsv(docid, (long)1);
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_TERM_COUNT::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------------
*/
void ANT_ranking_function_term_count::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, (long)1);
		}
	current++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#endif

/*
	ANT_RANKING_FUNCTION_TERM_COUNT::RANK()
	---------------------------------------
*/
double ANT_ranking_function_term_count::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
return (double)1;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
