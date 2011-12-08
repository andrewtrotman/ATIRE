/*
	RANKING_FUNCTION_DPH.C
	----------------------
*/
#ifdef _MSC_VER
	/*
		This is necessary for Microsoft C/C++ to include M_PI
	*/
	#define _USE_MATH_DEFINES
#endif
#include "maths.h"
#include "pragma.h"
#include "ranking_function_dph.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_DPH::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
*/
void ANT_ranking_function_DPH::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, cf, score;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
cf = (double)term_details->global_collection_frequency;
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++ * prescalar;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;

		double f = tf / document_lengths[(size_t)docid];
		double norm = (1.0 - f) * (1.0 - f) / (tf + 1.0);
		score = 1.0 * norm * (tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - f)));

		accumulator->add_rsv(docid, postscalar * score);
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_DPH::RANK()
	--------------------------------
	Not implemented *yet*
*/
double ANT_ranking_function_DPH::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
return 1.0;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
