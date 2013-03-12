/*
	RANKING_FUNCTION_PREGEN.C
	-------------------------
*/
#include <limits>
#include <math.h>
#include <stdlib.h>
#include "pragma.h"
#include "ranking_function_pregen.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_PREGEN::RELEVANCE_RANK_ONE_QUANTUM()
	---------------------------------------------------------
*/
void ANT_ranking_function_pregen::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
ANT_compressable_integer *current;
ANT_search_engine_accumulator::ANT_accumulator_t value;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	value = (ANT_search_engine_accumulator::ANT_accumulator_t) (ascending ? std::numeric_limits<ANT_pregen_t>::max() - pregen->scores[docid] : pregen->scores[docid]);
	quantum_parameters->accumulator->set_rsv(docid, value);
	}
}

/*
	ANT_RANKING_FUNCTION_PREGEN::RELEVANCE_RANK_TOP_K()
	---------------------------------------------------
*/
void ANT_ranking_function_pregen::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
ANT_compressable_integer *current, *end;
ANT_search_engine_accumulator::ANT_accumulator_t value;

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
		value = (ANT_search_engine_accumulator::ANT_accumulator_t)(ascending ? std::numeric_limits<ANT_pregen_t>::max() - pregen->scores[docid] : pregen->scores[docid]);
		accumulator->set_rsv(docid, value);
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else

/*
	ANT_RANKING_FUNCTION_PREGEN::RELEVANCE_RANK_TOP_K()
	---------------------------------------------------
*/
void ANT_ranking_function_pregen::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
ANT_compressable_integer *current, *end;
ANT_search_engine_accumulator::ANT_accumulator_t value;

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
		value = (ANT_search_engine_accumulator::ANT_accumulator_t)(ascending ? std::numeric_limits<ANT_pregen_t>::max() - pregen->scores[docid] : pregen->scores[docid]);
		accumulator->set_rsv(docid, value);
		}
	current++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#endif

/*
	ANT_RANKING_FUNCTION_PREGEN::RANK()
	-----------------------------------
*/
double ANT_ranking_function_pregen::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
exit(printf("Cannot compute a static rsv for a pregen because they are runtime plugable\n"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
