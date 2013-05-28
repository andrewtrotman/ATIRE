/*
	RANKING_FUNCTION_DFI.C
	----------------------
	Divergence from Independance model.  See:
	Bekir Taner DinÁer, Ilker Kocabas, Bahar Karaoglan (2010): IRRA at TREC 2010: Index Term Weighting by Divergence From Independence Model. TREC 2010
*/

#include "maths.h"
#include "pragma.h"
#include "ranking_function_dfi.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"


#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DFI::RELEVANCE_RANK_ONE_QUANTUM()
	-------------------------------------------------------
*/
void ANT_ranking_function_DFI::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_DFI::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
*/
void ANT_ranking_function_DFI::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else

/*
	ANT_RANKING_FUNCTION_DFI::RELEVANCE_RANK_TOP_K()
	------------------------------------------------


	            tf - ef
	rsv = log2(-------- + 1)
	           sqrt(ef)

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
*/
void ANT_ranking_function_DFI::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
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
		score = ANT_log2((tf - ef) / sqrt(ef) + 1);

//		if (score > 0)
		if (tf - ef > 0)
			accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DFI::RANK()
	--------------------------------
*/
double ANT_ranking_function_DFI::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
double tf = (double)term_frequency;
double cf = (double)collection_frequency;

double ef = cf * (length / cf);
double score = ANT_log2((tf - ef) / sqrt(ef) + 1);

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
