/*
	RANKING_FUNCTION_LMJM.C
	-----------------------
	Language Models with Jelinek-Mercer smoothing

	Probably the best reference is:
	C. Zhai, J. Lafferty (2004) A Study of Smoothing Methods for Language Models Applied to Information Retrieval, ACM Transactions on Information Systems, 22(2):17-214
	which is based on:
	C. Zhai, J. Lafferty (2001) A study of smoothing methods for language models applied to Ad Hoc information retrieval, Proceedings of SIGIR 2001, pp. 334-342
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_lmjm.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_LMJM::RELEVANCE_RANK_ONE_QUANTUM()
	-------------------------------------------------------
	Language Models with Jelinek-Mercer smoothing
*/
void ANT_ranking_function_lmjm::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double rsv;
double one_minus_lambda, idf;
ANT_compressable_integer *current;

one_minus_lambda = (1.0 - lambda) / lambda;
idf = (double)collection_length_in_terms / (double)quantum_parameters->term_details->global_collection_frequency;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	rsv = quantum_parameters->postscalar * log(1 + one_minus_lambda * (quantum_parameters->prescalar * quantum_parameters->tf / (double)document_lengths[(size_t)docid]) * idf);
	quantum_parameters->accumulator->add_rsv(docid, quantize(rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_LMJM::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
	Language Models with Jelinek-Mercer smoothing
*/
void ANT_ranking_function_lmjm::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, rsv;
double one_minus_lambda, idf;
ANT_compressable_integer *current, *end;

/*
								 1 - lambda   tf(dt)   len(c)
	 rsv = log(1 + ---------- * ------ * ------)
									 lambda     len(d)    cf(t)
*/
one_minus_lambda = (1.0 - lambda) / lambda;
idf = (double)collection_length_in_terms / (double)term_details->global_collection_frequency;
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
		rsv = postscalar * log(1 + one_minus_lambda * (prescalar * tf / (double)document_lengths[(size_t)docid]) * idf);
		accumulator->add_rsv(docid, quantize(rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_LMJM::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
	Language Models with Jelinek-Mercer smoothing
*/
void ANT_ranking_function_lmjm::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, rsv;
double one_minus_lambda, idf;
ANT_compressable_integer *current, *end;

/*
                 1 - lambda   tf(dt)   len(c)
   rsv = log(1 + ---------- * ------ * ------)
                   lambda     len(d)    cf(t)
*/
current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
one_minus_lambda = (1.0 - lambda) / lambda;
idf = (double)collection_length_in_terms / (double)term_details->global_collection_frequency;
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		rsv = postscalar * log(1 + one_minus_lambda * (prescalar * tf / (double)document_lengths[(size_t)docid]) * idf);
		accumulator->add_rsv(docid, quantize(rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_LMJM::RANK()
	---------------------------------
*/
double ANT_ranking_function_lmjm::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
double one_minus_lambda, idf, rsv;

one_minus_lambda = (1.0 - lambda) / lambda;
idf = (double)collection_length_in_terms / (double)collection_frequency;
rsv = log(1 + one_minus_lambda * ((double)term_frequency / (double)length) * idf);

return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
