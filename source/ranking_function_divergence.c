/*
	RANKING_FUNCTION_DIVERGENCE.C
	-----------------------------
	Divergence from Randomness I(ne)B2

	see G. Amati and C.J. van Rijsbergen (2002), Probabilistic Models of Information Retrieval Based on
	Measuring the Divergence from Randomness, Transactions on Information Systems 20(4):357-389.
*/
#include "pragma.h"
#include "maths.h"
#include "ranking_function_divergence.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DIVERGENCE::RELEVANCE_RANK_ONE_QUANTUM()
	-------------------------------------------------------------
*/
void ANT_ranking_function_divergence::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double tf, rsv, tf_prime, ne, F, F_plus_1, inf_right;
ANT_compressable_integer *current;

F = (double)quantum_parameters->term_details->global_collection_frequency;
F_plus_1 = F + 1.0;
ne = documents * (1.0 - pow((documents - 1.0) / documents, F));
inf_right = ANT_log2(((double)documents + 1.0) / (ne + 0.5));
tf = quantum_parameters->tf;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	tf_prime = quantum_parameters->prescalar * tf * ANT_log2(1.0 + (double)mean_document_length / (double)document_lengths[(size_t)docid]);
	rsv = tf_prime * inf_right * (F_plus_1 / ((double)quantum_parameters->term_details->global_document_frequency * (tf_prime + 1.0)));
	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_DIVERGENCE::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------------
*/
void ANT_ranking_function_divergence::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, rsv, tf_prime, ne, F, F_plus_1, inf_right;
ANT_compressable_integer *current, *end;

/*
	we are using I(ne)B2 defined thus:

															N + 1
	Inf1 = I(ne) = tf' * log2(--------)
														ne + 0.5

	where
								 N - 1
	ne = N * (1 - (-----)^F )
									 N

	where
	tf' = normalised term frequency (see below)
	N = Number of documents
	F = collection frequency

	and use First Normalisation B

										F + 1
	weight(t,d) = ------------- * Inf1
								n * (tf' + 1)

	where n = document frequency
	finally we length normalise tf using H2

										 av_len
	tf' = tf.log2(1 + ------)
											len

	where
		len is the length of the document,
		av_len is the average document length,
		ttf is the number of occurences of the term in the document
*/
F = (double)term_details->global_collection_frequency;
F_plus_1 = F + 1.0;
ne = documents * (1.0 - pow((documents - 1.0) / documents, F));
inf_right = ANT_log2(((double)documents + 1.0) / (ne + 0.5));

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
		tf_prime = prescalar * tf * ANT_log2(1.0 + (double)mean_document_length / (double)document_lengths[(size_t)docid]);
		rsv = tf_prime * inf_right * (F_plus_1 / ((double)term_details->global_document_frequency * (tf_prime + 1.0)));
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
void ANT_ranking_function_divergence::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, rsv, tf_prime, ne, F, F_plus_1, inf_right;
ANT_compressable_integer *current, *end;

/*
	we are using I(ne)B2 defined thus:

	                            N + 1
	Inf1 = I(ne) = tf' * log2(--------)
	                          ne + 0.5

	where
	               N - 1
	ne = N * (1 - (-----)^F )
	                 N

	where
	tf' = normalised term frequency (see below)
	N = Number of documents
	F = collection frequency

	and use First Normalisation B

	                  F + 1
	weight(t,d) = ------------- * Inf1
	              n * (tf' + 1)

	where n = document frequency
	finally we length normalise tf using H2

	                   av_len
	tf' = tf.log2(1 + ------)
	                    len

	where
		len is the length of the document,
		av_len is the average document length,
		ttf is the number of occurences of the term in the document
*/
current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);

F = (double)term_details->global_collection_frequency;
F_plus_1 = F + 1.0;
ne = documents * (1.0 - pow((documents - 1.0) / documents, F));
inf_right = ANT_log2(((double)documents + 1.0) / (ne + 0.5));

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		tf_prime = prescalar * tf * ANT_log2(1.0 + (double)mean_document_length / (double)document_lengths[(size_t)docid]);
		rsv = tf_prime * inf_right * (F_plus_1 / ((double)term_details->global_document_frequency * (tf_prime + 1.0)));
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DIVERGENCE::RANK()
	---------------------------------------
*/
double ANT_ranking_function_divergence::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
double F, F_plus_1, ne, inf_right, tf, tf_prime, rsv;

F = (double)collection_frequency;
F_plus_1 = F + 1.0;
ne = documents * (1.0 - pow((documents - 1.0) / documents, F));
inf_right = ANT_log2(((double)documents + 1.0) / (ne + 0.5));

tf = term_frequency;
tf_prime = tf * ANT_log2(1.0 + (double)mean_document_length / (double)length);
rsv = tf_prime * inf_right * (F_plus_1 / ((double)document_frequency * (tf_prime + 1.0)));
return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
