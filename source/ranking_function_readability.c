/*
	RANKING_FUNCTION_READABILITY.C
	------------------------------
*/
#include <math.h>
#include "ranking_function_readability.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"
#include "search_engine_readability.h"

/*
	ANT_RANKING_FUNCTION_READABILITY::ANT_RANKING_FUNCTION_READABILITY()
	--------------------------------------------------------------------
*/
ANT_ranking_function_readability::ANT_ranking_function_readability(ANT_search_engine_readability *engine, double mix, double k1, double b) : ANT_ranking_function(engine)
{
this->mix = mix;
this->k1 = k1;
this->b = b;
this->document_readability = engine->document_readability;
this->hardest_document = engine->hardest_document;
}

/*
	ANT_RANKING_FUNCTION_READABILITY::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------------
*/
void ANT_ranking_function_readability::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
const double k1_plus_1 = k1 + 1.0;
const double one_minus_b = 1.0 - b;
double bm25;
long docid;
double top_row, tf, idf, readability;
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
		bm25 = (idf * (top_row / (tf + k1 * (one_minus_b + b * (document_lengths[docid] / mean_document_length)))));
		readability = (hardest_document - document_readability[docid]) / 1000.0;

		//accumulator[docid].add_rsv((mix * bm25) + ((1 - mix) * (20000.0 - document_readability[docid]) / 1000.0));
		//accumulator[docid].add_rsv((hardest_document - document_readability[docid]) / 1000.0);
		accumulator[docid].add_rsv((mix * bm25) + ((1.0 - mix) * readability));
		}
	current++;		// skip over the zero
	}
}

