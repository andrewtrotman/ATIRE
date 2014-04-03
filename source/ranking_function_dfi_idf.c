/*
	RANKING_FUNCTION_DFI_IDF.C
	--------------------------
	Divergence from Independance model with IDF
*/

#include "maths.h"
#include "pragma.h"
#include "ranking_function_dfi_idf.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DFI_IDF::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------
*/
void ANT_ranking_function_DFI_IDF::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double cf, tf, ef, rsv;
ANT_compressable_integer *current;

/*
	           tf - ef              c
	rsv = log2(-------- + 1) * log2(--)
	           sqrt(ef)             ci

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
	c is the number of documents in the collection
	ci is the number of documents containing the term
*/

cf = (double)quantum_parameters->term_details->global_collection_frequency;
tf = quantum_parameters->tf * quantum_parameters->prescalar;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;

	ef = cf * ((double)document_lengths[(size_t)docid] / (double)collection_length_in_terms);
	rsv = ANT_log2((tf - ef) / sqrt(ef) + 1) * ANT_log2((double)documents / (double)quantum_parameters->term_details->global_document_frequency);

	if (tf - ef > 0)
		quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_DFI_IFD::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_DFI_IDF::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double cf, tf, ef, rsv;
ANT_compressable_integer *current, *end;

/*
	           tf - ef              c
	rsv = log2(-------- + 1) * log2(--)
	           sqrt(ef)             ci

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
	c is the number of documents in the collection
	ci is the number of documents containing the term
*/
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

		ef = cf * ((double)document_lengths[(size_t)docid] / (double)collection_length_in_terms);
		rsv = ANT_log2((tf - ef) / sqrt(ef) + 1) * ANT_log2((double)documents / (double)term_details->global_document_frequency);

		if (tf - ef > 0)
			accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
}

#else
/*
	ANT_RANKING_FUNCTION_DFI_IDF::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------


	           tf - ef              c
	rsv = log2(-------- + 1) * log2(--)
	           sqrt(ef)             ci

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
	c is the number of documents in the collection
	ci is the number of documents containing the term
*/
void ANT_ranking_function_DFI_IDF::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
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
		score = ANT_log2((tf - ef) / sqrt(ef) + 1) * ANT_log2((double)documents / (double)term_details->global_document_frequency);

//		if (score > 0)
		if (tf - ef > 0)
			accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DFI_IDF::RANK()
	------------------------------------
*/
double ANT_ranking_function_DFI_IDF::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
double tf = (double)term_frequency;
double cf = (double)collection_frequency;

double ef = cf * ((double)length / cf);
double score = ANT_log2((tf - ef) / sqrt(ef) + 1) * ANT_log2((double)documents / (double)document_frequency);

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
