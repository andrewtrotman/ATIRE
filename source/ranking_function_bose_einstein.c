/*
	RANKING_FUNCTION_BOSE_EINSTEIN.C
	--------------------------------
*/
#include <stdio.h>
#include <math.h>
#include <limits>
#include "ranking_function_bose_einstein.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------------
	Ranking function GL2 from:
	G. Amati and C.J. van Rijsbergen (2002), Probabilistic Models of Information Retrieval Based on 
	Measuring the Divergence from Randomness, Transactions on Information Systems 20(4):357-389.
*/
void ANT_ranking_function_bose_einstein::relevance_rank_top_k(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid;
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
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);

left = log(1.0 + (double)term_details->collection_frequency / (double)documents);
right = log(1.0 + (double)documents / (double)term_details->collection_frequency);

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		tf_prime = tf * log(1.0 + mean_document_length / (double)document_lengths[docid]);
		rsv = (left + tf_prime * right) / (tf_prime + 1.0);

		accumulator[docid].add_rsv(rsv);
		}
	current++;		// skip over the zero
	}
}


/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::GET_MAX_MIN()
	-------------------------------------------------
*/
void ANT_ranking_function_bose_einstein::get_max_min(double *maximum, double *minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned char *term_frequencies)
{
long docid;
double tf, rsv, left, right, tf_prime;
unsigned char *current_tf, *end;
ANT_compressable_integer *current_docid;

left = log(1.0 + (double)collection_frequency / documents);
right = log(1.0 + documents / (double)collection_frequency);

current_tf = term_frequencies;
current_docid = document_ids;
end = term_frequencies + document_frequency;

docid = -1;
while (current_tf < end)
	{
	tf = *current_tf;
	docid += *current_docid;

	tf_prime = tf * log(1.0 + mean_document_length / (double)document_lengths[docid]);
	rsv = (left + tf_prime * right) / (tf_prime + 1.0);

	if (rsv > *maximum)
		*maximum = rsv;
	if (rsv < *minimum)
		*minimum = rsv;

	current_tf++;
	current_docid++;
	}
}

/*
	ANT_RANKING_FUNCTION_BOSE_EINSTEIN::QUANTIZE()
	----------------------------------------------
*/
void ANT_ranking_function_bose_einstein::quantize(double maximum, double minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned char *term_frequencies)
{
long docid;
double tf, rsv, left, right, tf_prime, range;
unsigned char *current_tf, *end;
ANT_compressable_integer *current_docid;

range = maximum - minimum;
left = log(1.0 + (double)collection_frequency / documents);
right = log(1.0 + documents / (double)collection_frequency);

current_tf = term_frequencies;
current_docid = document_ids;
end = term_frequencies + document_frequency;

docid = -1;
while (current_tf < end)
	{
	tf = *current_tf;
	docid += *current_docid;

	tf_prime = tf * log(1.0 + mean_document_length / (double)document_lengths[docid]);
	rsv = (left + tf_prime * right) / (tf_prime + 1.0);

	*current_tf = (unsigned char)(((rsv - minimum) / range) * 0xFE) + 1;			// change the tf value into an impact value

	current_tf++;
	current_docid++;
	}
}
