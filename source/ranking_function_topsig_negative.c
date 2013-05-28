/*
	RANKING_FUNCTION_TOPSIG_NEGATIVE.C
	----------------------------------
	This is the ranking function that gets called when the topsig has a negative value for a
	particular dimension.  In this case it must use the 0-values in the bitstring.  Since the
	encoding is tf=1 for all 1 bits, the 0 bits must be all documents not in the first impact
	set of the postings list.

	The Topsig paper is currently unpublished, but:
	Geva, S., De Vries, C. TOPSIG: Topology Preserving Document Signatures, Proceedings of CIKM 2100
*/
#include <math.h>
#include <stdlib.h>
#include "pragma.h"
#include "ranking_function_topsig_negative.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_TOPSIG_NEGATIVE::RELEVANCE_RANK_ONE_QUANTUM()
	------------------------------------------------------------------
*/
void ANT_ranking_function_topsig_negative::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid, start;
ANT_compressable_integer *current;

start = 0;
docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	while (start < docid)
		{
		if (quantum_parameters->accumulator->is_zero_rsv(start))
			quantum_parameters->accumulator->add_rsv(start, (long)1 + document_prior_probability[start]);
		else
			quantum_parameters->accumulator->add_rsv(start, (long)1);
		start++;
		}
	start = docid + 1;
	}
}

/*
	ANT_RANKING_FUNCTION_TOPSIG_NEGATIVE::RELEVANCE_RANK_TOP_K()
	------------------------------------------------------------
*/
void ANT_ranking_function_topsig_negative::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid, start;
ANT_compressable_integer *current, *end;

start = 0;
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
		while (start < docid)
			{
			if (accumulator->is_zero_rsv(start))
				accumulator->add_rsv(start, (long)1 + document_prior_probability[start]);
			else
				accumulator->add_rsv(start, (long)1);
			start++;
			}
		start = docid + 1;
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}

/*
	Now catch the documents after the last posting
*/
while (start < documents_as_integer)
	{
	if (accumulator->is_zero_rsv(start))
		accumulator->add_rsv(start, (long)1 + document_prior_probability[start]);
	else
		accumulator->add_rsv(start, (long)1);
	start++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_TOPSIG_NEGATIVE::RELEVANCE_RANK_TOP_K()
	------------------------------------------------------------
*/
void ANT_ranking_function_topsig_negative::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid, start;
ANT_compressable_integer *current, *end;

current = impact_ordering;
start = 0;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		while (start < docid)
			{
			if (accumulator->is_zero_rsv(start))
				accumulator->add_rsv(start, (long)1 + document_prior_probability[start]);
			else
				accumulator->add_rsv(start, (long)1);
			start++;
			}
		start = docid + 1;
		}
	current++;
	}

/*
	Now catch the documents after the last posting
*/
while (start < documents_as_integer)
	{
	if (accumulator->is_zero_rsv(start))
		accumulator->add_rsv(start, (long)1 + document_prior_probability[start]);
	else
		accumulator->add_rsv(start, (long)1);
	start++;
	}

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#endif

/*
	ANT_RANKING_FUNCTION_TOPSIG_NEGATIVE::RANK()
	--------------------------------------------
*/
double ANT_ranking_function_topsig_negative::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
exit(printf("Cannot pre-compute the topsig collating sequence\n"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
