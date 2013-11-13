/*
	RANKING_FUNCTION_BOSE_EINSTEIN.C
	--------------------------------
*/
#include <stdio.h>
#include <math.h>
#include <limits>
#include "pragma.h"
#include "ranking_function_bose_einstein.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------------
	Ranking function GL2 from:
	G. Amati and C.J. van Rijsbergen (2002), Probabilistic Models of Information Retrieval Based on
	Measuring the Divergence from Randomness, Transactions on Information Systems 20(4):357-389.
*/
void ANT_ranking_function_bose_einstein::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double rsv, left, right, tf_prime;
ANT_compressable_integer *current;

left = log(1.0 + (double)quantum_parameters->term_details->global_collection_frequency / (double)documents);
right = log(1.0 + (double)documents / (double)quantum_parameters->term_details->global_collection_frequency);
docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	tf_prime = quantum_parameters->prescalar * quantum_parameters->tf * log(1.0 + mean_document_length / (double)document_lengths[(size_t)docid]);
	rsv = (left + tf_prime * right) / (tf_prime + 1.0);
	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------------
	Ranking function GL2 from:
	G. Amati and C.J. van Rijsbergen (2002), Probabilistic Models of Information Retrieval Based on
	Measuring the Divergence from Randomness, Transactions on Information Systems 20(4):357-389.
*/
void ANT_ranking_function_bose_einstein::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, rsv, left, right, tf_prime;
ANT_compressable_integer *current, *end;

/*
				log(1 + cf(t) / N) + tf'(td) * log (1 + N / cf(t))
	rsv = --------------------------------------------------
													tf'(td) + 1

	where
		tf'(td) = tf(td) * log(1 + av_len_d / len(d))


	This implementation has:
		Inf =according to the Geometric estimation of the Bose Einstein Model of Randomness
		First Normalisation according to Laplace's Law of Succession
		Second (length) normalisation according to H2
	and is therefore GL2

*/
left = log(1.0 + (double)term_details->global_collection_frequency / (double)documents);
right = log(1.0 + (double)documents / (double)term_details->global_collection_frequency);
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
		tf_prime = prescalar * tf * log(1.0 + mean_document_length / (double)document_lengths[(size_t)docid]);
		rsv = (left + tf_prime * right) / (tf_prime + 1.0);
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
void ANT_ranking_function_bose_einstein::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, rsv, left, right, tf_prime;
ANT_compressable_integer *current, *end;

/*
	      log(1 + cf(t) / N) + tf'(td) * log (1 + N / cf(t))
	rsv = --------------------------------------------------
	                        tf'(td) + 1

	where
		tf'(td) = tf(td) * log(1 + av_len_d / len(d))


	This implementation has:
		Inf =according to the Geometric estimation of the Bose Einstein Model of Randomness
		First Normalisation according to Laplace's Law of Succession
		Second (length) normalisation according to H2
	and is therefore GL2

*/
current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);

left = log(1.0 + (double)term_details->global_collection_frequency / (double)documents);
right = log(1.0 + (double)documents / (double)term_details->global_collection_frequency);

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		tf_prime = prescalar * tf * log(1.0 + mean_document_length / (double)document_lengths[(size_t)docid]);
		rsv = (left + tf_prime * right) / (tf_prime + 1.0);

		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::RANK()
	------------------------------------------
*/
double ANT_ranking_function_bose_einstein::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
double tf, rsv, left, right, tf_prime;

left = log(1.0 + (double)collection_frequency / (double)documents);
right = log(1.0 + (double)documents / (double)collection_frequency);

tf = term_frequency;
tf_prime = tf * log(1.0 + mean_document_length / (double)length);
rsv = (left + tf_prime * right) / (tf_prime + 1.0);
return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
