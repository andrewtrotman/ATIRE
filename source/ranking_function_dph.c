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

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DPH::RELEVANCE_RANK_ONE_QUANTUM()
	------------------------------------------------------
*/
void ANT_ranking_function_DPH::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double f, norm, tf, cf, score;
ANT_compressable_integer *current;

cf = (double)quantum_parameters->term_details->global_collection_frequency;
tf = quantum_parameters->tf;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	f = tf / document_lengths[(size_t)docid];
	norm = (1.0 - f) * (1.0 - f) / (tf + 1.0);
	score = 1.0 * norm * (tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - f)));

	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_DPH::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
*/
void ANT_ranking_function_DPH::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double f, norm, tf, cf, score;
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
		f = tf / document_lengths[(size_t)docid];
		norm = (1.0 - f) * (1.0 - f) / (tf + 1.0);
		score = 1.0 * norm * (tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - f)));

		accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
void ANT_ranking_function_DPH::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double f, norm, tf, cf, score;
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

		f = tf / document_lengths[(size_t)docid];
		norm = (1.0 - f) * (1.0 - f) / (tf + 1.0);
		score = 1.0 * norm * (tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - f)));

		accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DPH::RANK()
	--------------------------------
*/
double ANT_ranking_function_DPH::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
double f, norm, score;

f = (double)term_frequency / (double)length;
norm = (1.0 - f) * (1.0 - f) / (term_frequency + 1.0);

score = 1.0 * norm * ((double)term_frequency * ANT_log2(((double)term_frequency * (double)mean_document_length / (double)length) * ((double)documents / (double)collection_frequency)) + 0.5 * ANT_log2(2.0 * M_PI * term_frequency * (1.0 - f)));

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
