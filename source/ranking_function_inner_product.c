/*
	RANKING_FUNCTION_INNER_PRODUCT.C
	--------------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_inner_product.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_INNER_PRODUCT::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------------
*/
void ANT_ranking_function_inner_product::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double idf;
ANT_compressable_integer *current;

idf = log((double)documents / (double)quantum_parameters->term_details->global_document_frequency);

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * (quantum_parameters->prescalar * quantum_parameters->tf * idf * idf) / 100.0, maximum_collection_rsv, minimum_collection_rsv));  // TF.IDF scores blow-out the integer accumulators and so we shift the decimal place back a bit
	}
}

/*
	ANT_RANKING_FUNCTION_INNER_PRODUCT::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------------
*/
void ANT_ranking_function_inner_product::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, idf;
ANT_compressable_integer *current, *end;

/*
						N
	IDF = log -
						n
*/
idf = log((double)documents / (double)term_details->global_document_frequency);

/*
	rsv = (tf_in_d * IDF) * (tf_in_q * IDF)

	where tf_in_d is the tf in the document and tf_in_q is 1 and so we get tf * IDF * IDF
*/
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr;
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		accumulator->add_rsv(docid, quantize(postscalar * (prescalar * tf * idf * idf) / 100.0, maximum_collection_rsv, minimum_collection_rsv));  // TF.IDF scores blow-out the integer accumulators and so we shift the decimal place back a bit
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_INNER_PRODUCT::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------------
*/
void ANT_ranking_function_inner_product::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, idf;
ANT_compressable_integer *current, *end;

/*
	          N
	IDF = log -
	          n
*/

idf = log((double)documents / (double)term_details->global_document_frequency);

/*
	rsv = (tf_in_d * IDF) * (tf_in_q * IDF)

	where tf_in_d is the tf in the document and tf_in_q is 1 and so we get tf * IDF * IDF
*/
current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, quantize(postscalar * (prescalar * tf * idf * idf) / 100.0, maximum_collection_rsv, minimum_collection_rsv));  // TF.IDF scores blow-out the integer accumulators and so we shift the decimal place back a bit
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_INNER_PRODUCT::RANK()
	------------------------------------------
*/
double ANT_ranking_function_inner_product::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
double idf, tf, rsv;

tf = (double)term_frequency;
idf = log((double)documents / (double)document_frequency);
rsv = tf * idf * idf;

return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
