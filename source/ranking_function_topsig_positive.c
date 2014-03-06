/*
	RANKING_FUNCTION_TOPSIG_POSITIVE.C
	----------------------------------
	This ranking function gets called by the TopSig code in the case of a positive value in a give dimenstion.

	The Topsig paper is currently unpublished, but:
	Geva, S., De Vries, C. TOPSIG: Topology Preserving Document Signatures, Proceedings of CIKM 2100
*/
#include <math.h>
#include <stdlib.h>
#include "pragma.h"
#include "ranking_function_topsig_positive.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"


#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_TOPSIG_POSITIVE::RELEVANCE_RANK_ONE_QUANTUM()
	------------------------------------------------------------------
*/
void ANT_ranking_function_topsig_positive::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
ANT_compressable_integer *current;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	if (quantum_parameters->accumulator->is_zero_rsv(docid))
		quantum_parameters->accumulator->add_rsv(docid, (long)1 + document_prior_probability[docid]);
	else
		quantum_parameters->accumulator->add_rsv(docid, (long)1);
	}
}

/*
	ANT_RANKING_FUNCTION_TOPSIG_POSITIVE::RELEVANCE_RANK_TOP_K()
	------------------------------------------------------------
*/
void ANT_ranking_function_topsig_positive::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
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
		if (accumulator->is_zero_rsv(docid))
			accumulator->add_rsv(docid, (long)1 + document_prior_probability[docid]);
		else
			accumulator->add_rsv(docid, (long)1);
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
void ANT_ranking_function_topsig_positive::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
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
		if (accumulator->is_zero_rsv(docid))
			accumulator->add_rsv(docid, (long)1 + document_prior_probability[docid]);
		else
			accumulator->add_rsv(docid, (long)1);
		}
	current++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#endif

/*
	ANT_RANKING_FUNCTION_TOPSIG_POSITIVE::RANK()
	--------------------------------------------
*/
double ANT_ranking_function_topsig_positive::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
exit(printf("Cannot pre-compute the topsig collating sequence\n"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
