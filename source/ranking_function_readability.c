/*
	RANKING_FUNCTION_READABILITY.C
	------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pragma.h"
#include "ranking_function_readability.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"
#include "search_engine_readability.h"

/*
	ANT_RANKING_FUNCTION_READABILITY::ANT_RANKING_FUNCTION_READABILITY()
	--------------------------------------------------------------------
*/
ANT_ranking_function_readability::ANT_ranking_function_readability(ANT_search_engine_readability *engine, double mix, double cutoff, double k1, double b) : ANT_ranking_function(engine)
{
this->cutoff = cutoff ? cutoff : engine->hardest_document / 1000.0;
this->mix = mix;
this->k1 = k1;
this->b = b;

this->document_readability = engine->document_readability;
}

/*
	ANT_RANKING_FUNCTION_READABILITY::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------------
*/
void ANT_ranking_function_readability::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
const double k1_plus_1 = k1 + 1.0;
const double one_minus_b = 1.0 - b;
long docid;
double top_row, tf, idf;
ANT_compressable_integer *current, *end;

idf = log((double)(documents) / (double)term_details->document_frequency);
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);		// allow early termination
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	top_row = tf * k1_plus_1;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		
		/*
			Add the readability for this document - because a low readability score is better, we add the hardest document minus the actual score.
			We also only want to only consider the readability once per document.
		*/
		if (accumulator->is_zero_rsv(docid))
			accumulator->add_rsv(docid, (1.0 - mix) * (cutoff - (document_readability[docid] / 1000.0)));
		
		/*
			Add the portion of BM25 for this query term
		*/
		accumulator->add_rsv(docid, mix * (idf * (top_row / (tf + k1 * (one_minus_b + b * (document_lengths[docid] / mean_document_length))))));
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_READABILITY::RANK()
	----------------------------------------
*/
double ANT_ranking_function_readability::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
exit(printf("Cannot pre-compute the impact of the readability similarity function (it is meaningless)\n"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
