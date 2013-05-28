/*
	RANKING_FUNCTION_DFREE.C
	------------------------
*/
#ifdef _MSC_VER
	/*
		This is necessary for Microsoft C/C++ to include M_PI
	*/
	#define _USE_MATH_DEFINES
#endif
#include "maths.h"
#include "pragma.h"
#include "ranking_function_dfree.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"


#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DFREE::RELEVANCE_RANK_ONE_QUANTUM()
	--------------------------------------------------------
*/
void ANT_ranking_function_DFRee::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double prior, posterior, InvPriorCollection, norm;
double tf, cf, score;
ANT_compressable_integer *current;

cf = (double)quantum_parameters->term_details->global_collection_frequency;
tf = quantum_parameters->tf;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	prior = tf / document_lengths[(size_t)docid];
	posterior = (tf + 1.0) / (document_lengths[(size_t)docid] + 1);
	InvPriorCollection = collection_length_in_terms / tf;
	norm = tf * ANT_log2(posterior / prior);

	score = 1.0 * norm * (tf * (-ANT_log2(prior * InvPriorCollection)) + (tf + 1.0) *  (+ANT_log2(posterior * InvPriorCollection)) + 0.5 * ANT_log2(posterior / prior));

	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_DFREE::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------
*/
void ANT_ranking_function_DFRee::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double prior, posterior, InvPriorCollection, norm;
double tf, cf, score;
ANT_compressable_integer *current, *end;

cf = (double)term_details->global_collection_frequency;
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr * prescalar;
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;

		prior = tf / document_lengths[(size_t)docid];
		posterior = (tf + 1.0) / (document_lengths[(size_t)docid] + 1);
		InvPriorCollection = collection_length_in_terms / tf;
		norm = tf * ANT_log2(posterior / prior);

		score = 1.0 * norm * (tf * (-ANT_log2(prior * InvPriorCollection)) + (tf + 1.0) *  (+ANT_log2(posterior * InvPriorCollection)) + 0.5 * ANT_log2(posterior / prior));

		accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
void ANT_ranking_function_DFRee::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double prior, posterior, InvPriorCollection, norm;
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

		prior = tf / document_lengths[(size_t)docid];
		posterior = (tf + 1.0) / (document_lengths[(size_t)docid] + 1);
		InvPriorCollection = collection_length_in_terms / tf;
		norm = tf * ANT_log2(posterior / prior);

		score = 1.0 * norm * (tf * (-ANT_log2(prior * InvPriorCollection)) + (tf + 1.0) *  (+ANT_log2(posterior * InvPriorCollection)) + 0.5 * ANT_log2(posterior / prior));

		accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DFREE::RANK()
	----------------------------------
*/
double ANT_ranking_function_DFRee::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
double tf = term_frequency;
double prior = tf / (double)length;
double posterior = (tf + 1.0) / ((double)length + 1);
double InvPriorCollection = collection_length_in_terms / tf;
double norm = tf * ANT_log2(posterior / prior);

double score = 1.0 * norm * (tf * (-ANT_log2(prior * InvPriorCollection)) + (tf + 1.0) *  (+ANT_log2(posterior * InvPriorCollection)) + 0.5 * ANT_log2(posterior / prior));

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
