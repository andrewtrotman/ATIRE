/*
	RANKING_FUNCTION_DFIW_IDF.C
	--------------------------
	Divergence from Independance model with Web correction and IDF
*/

#include "maths.h"
#include "pragma.h"
#include "ranking_function_dfiw_idf.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DFIW::RELEVANCE_RANK_ONE_QUANTUM()
	-------------------------------------------------------
*/
void ANT_ranking_function_DFIW_IDF::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters) {
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_DFI_IFW::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
*/
void ANT_ranking_function_DFIW_IDF::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar) {
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_DFIW_IDF::RELEVANCE_RANK_TOP_K()
	-----------------------------------------------------
*/
void ANT_ranking_function_DFIW_IDF::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, cf, ef, score;
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

		ef = cf * ((double)document_lengths[(size_t)docid] / (double)collection_length_in_terms);
		score = ANT_log2((tf - ef) / sqrt(ef) + 1) * ANT_log2((double)documents / (double)term_details->global_document_frequency) * -ANT_log2(tf / (double)document_lengths[(size_t)docid]);

//		if (score > 0)
		if (tf - ef > 0)
			accumulator->add_rsv(docid, postscalar * score);
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DFIW_IDF::RANK()
	-------------------------------------
*/
double ANT_ranking_function_DFIW_IDF::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
double tf = (double)term_frequency;
double cf = (double)collection_frequency;

double ef = cf * ((double)length / cf);
double score = ANT_log2((tf - ef) / sqrt(ef) + 1) * ANT_log2((double)documents / (double)document_frequency) * -ANT_log2(tf / (double)length);

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
