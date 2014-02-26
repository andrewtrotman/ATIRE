/*
	RANKING_FUNCTION_KBTFIDF.C
	--------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_kbtfidf.h"
#include "compress.h"
#include "search_engine_btree_leaf.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_KBTFIDF::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------
*/
void ANT_ranking_function_kbtfidf::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
double tf, idf, score;
long long docid;
ANT_compressable_integer *current;

idf = log((double)documents / (double)quantum_parameters->term_details->global_document_frequency);
tf = quantum_parameters->prescalar * quantum_parameters->tf;
score = quantize(quantum_parameters->postscalar * (log(k * tf - b) * idf * idf) / 100.0, maximum_collection_rsv, minimum_collection_rsv);

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	//quantum_parameters->accumulator->add_rsv(docid, quantum_parameters->postscalar * idf * (top_row / (quantum_parameters->prescalar * quantum_parameters->tf + document_prior_probability[(size_t)docid])));
	/*
		This code is Shlomo's W(t) ranking function.

		idf = log((tf / document_lengths[docid]) / (term_details->collection_frequency / collection_length_in_terms));
		if (idf > 0)
			{
			score = tf * idf;
			accumulator->add_rsv(docid, score);
			}
	*/
	quantum_parameters->accumulator->add_rsv(docid, score);
	}
}

/*
	ANT_RANKING_FUNCTION_KBTFIDF::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_kbtfidf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
double tf, idf, score;
long long docid;
ANT_compressable_integer *current, *end;

idf = log((double)documents / (double)term_details->global_document_frequency);
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	//tf = *impact_header->impact_value_ptr;
	tf = prescalar * *impact_header->impact_value_ptr;
	score = quantize(postscalar * (log(k * tf - b) * idf * idf) / 100.0, maximum_collection_rsv, minimum_collection_rsv);
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		/*
			This code is Shlomo's W(t) ranking function.

			idf = log((tf / document_lengths[docid]) / (term_details->collection_frequency / collection_length_in_terms));
			if (idf > 0)
				{
				score = tf * idf;
				accumulator->add_rsv(docid, score);
				}
		*/
		accumulator->add_rsv(docid, score);
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_KBTFIDF::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_kbtfidf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
double tf, idf, score;
long long docid;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
idf = log((double)documents / (double)term_details->global_document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = prescalar * *current++;
	score = quantize(postscalar * (log(k * tf - b) * idf * idf) / 100.0, maximum_collection_rsv, minimum_collection_rsv);
	docid = -1;

	while (*current != 0)
		{
		docid += *current++;
		/*
			This code is Shlomo's W(t) ranking function.

			idf = log((tf / document_lengths[docid]) / (term_details->collection_frequency / collection_length_in_terms));
			if (idf > 0)
				{
				score = tf * idf;
				accumulator->add_rsv(docid, score);
				}
		*/
		accumulator->add_rsv(docid, score);
		}
	current++;		// skip over the zero
	}
}
#endif
/*
	ANT_RANKING_FUNCTION_KBTFIDF::RANK()
	------------------------------------
*/
double ANT_ranking_function_kbtfidf::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
double idf, tf, rsv;

tf = (double)term_frequency;
idf = log((double)documents / (double)document_frequency);

rsv = log(k * tf - b) * idf * idf;

return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
