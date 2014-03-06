/*
	RANKING_FUNCTION_LMD.C
	----------------------
	Language Models with Dirichlet smoothing

	Probably the best reference is:
	C. Zhai, J. Lafferty (2004) A Study of Smoothing Methods for Language Models Applied to Information Retrieval, ACM Transactions on Information Systems, 22(2):17-214
	which is based on:
	C. Zhai, J. Lafferty (2001) A study of smoothing methods for language models applied to Ad Hoc information retrieval, Proceedings of SIGIR 2001, pp. 334-342
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_lmd.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_LMD::RELEVANCE_RANK_ONE_QUANTUM()
	------------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmd::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double idf, n;
double left_hand_side, rsv;
ANT_compressable_integer *current;

n = quantum_parameters->accumulator->get_term_count();
idf = ((double)collection_length_in_terms / (double)quantum_parameters->term_details->global_collection_frequency);
left_hand_side = log (1.0 + (quantum_parameters->prescalar * quantum_parameters->tf / u) * idf);

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	rsv = left_hand_side - n * log(1.0 + ((double)document_lengths[(size_t)docid] / u));
	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_LMD::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmd::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, idf, n;
double left_hand_side, rsv;
ANT_compressable_integer *current, *end;

/*
	               tf(td)   len(c)              len(d)
	 rsv = log(1 + ------ * ------) - n log(1 + ------)
	                 u     cf(t)                 u

	where  len(c) is the length of the collection (in terms), len(d) is the length of the document
	tf(td) is the term frequency of the term and cf(t) is the collection_frequency of the term
	and n is the length of the querty in terms.
*/
n = accumulator->get_term_count();
idf = ((double)collection_length_in_terms / (double)term_details->global_collection_frequency);
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr;
	left_hand_side = log (1.0 + (prescalar * tf / u) * idf);
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		rsv = left_hand_side - n * log(1.0 + ((double)document_lengths[(size_t)docid] / u));
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION_LMD::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmd::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, idf, n;
double left_hand_side, rsv;
ANT_compressable_integer *current, *end;

/*
                 tf(td)   len(c)              len(d)
   rsv = log(1 + ------ * ------) - n log(1 + ------)
                    u     cf(t)                 u

	where  len(c) is the length of the collection (in terms), len(d) is the length of the document
	tf(td) is the term frequency of the term and cf(t) is the collection_frequency of the term
	and n is the length of the querty in terms.
*/
n = accumulator->get_term_count();
current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
idf = ((double)collection_length_in_terms / (double)term_details->global_collection_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	left_hand_side = log (1.0 + (prescalar * tf / u) * idf);
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		rsv = left_hand_side - n * log(1.0 + ((double)document_lengths[(size_t)docid] / u));
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_LMD::RANK()
	--------------------------------
*/
double ANT_ranking_function_lmd::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
exit(printf("Cannot pre-compute the impact score of the LMD language model as it truely depends on query length and query frequencies"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
