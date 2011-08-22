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
#include "pragma.h"
#include "ranking_function_topsig_negative.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

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
			if (accumulator->is_zero_rsv(docid))
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
	accumulator->add_rsv(start, (long)1);
	start++;
	}

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_TOPSIG_NEGATIVE::RANK()
	--------------------------------------------
*/
double ANT_ranking_function_topsig_negative::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
exit(printf("Cannot pre-compute the topsig collating sequence\n"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
