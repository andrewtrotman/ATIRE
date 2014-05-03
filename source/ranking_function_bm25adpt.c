/*
	RANKING_FUNCTION_BM25ADPT.C
	---------------------------
	Implementaiton of BM25-adpt.  See: Y. Lv, CX. Zhai (2011) Adaptive Term Frequency Noramlization for BM25, Proceedings of CIKM 2011, pp. 1985-1988
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "maths.h"
#include "pragma.h"
#include "ranking_function_bm25adpt.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BM25ADPT::ANT_RANKING_FUNCTION_BM25ADPT()
	--------------------------------------------------------------
*/
ANT_ranking_function_BM25ADPT::ANT_ranking_function_BM25ADPT(ANT_search_engine *engine, long quantize, long long quantization_bits, double b) : ANT_ranking_function(engine, quantize, quantization_bits)
{
this->b = b;
}

/*
	ANT_RANKING_FUNCTION_BM25ADPT::ANT_RANKING_FUNCTION_BM25ADPT()
	--------------------------------------------------------------
*/
ANT_ranking_function_BM25ADPT::ANT_ranking_function_BM25ADPT(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double b) : ANT_ranking_function(documents, document_lengths, quantization_bits)
{
this->b = b;
}

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_BM25ADPT::RELEVANCE_RANK_ONE_QUANTUM()
	-----------------------------------------------------------
*/
void ANT_ranking_function_BM25ADPT::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
exit(printf("Cannot compute BM25ADPT quantum at a time (at the moment)\n"));
}

/*
	ANT_RANGE_SEARCH()
	------------------
*/
static inline double ANT_range_search(double end, double err, double (*function)(double, void *parameter), void *function_parameter)
{
double best_score, score, jump, where, new_score;

best_score = DBL_MAX;
jump = 1;
where = err;
do
	{
	if (where > fabs(jump) + end)
		break;
	score = function(where, function_parameter);
	if (score > best_score)
		{
		jump /= 2;
		/*
			but we don't know if its before us or after us so we need to check the sign
		*/
		if (where + jump < 0)
			jump = -jump;

		new_score = function(where + jump, function_parameter);

		if (new_score > score)
			jump = -jump;
		else
			{
			where += jump;
			best_score = new_score;
			}
		}
	else
		{
		where += jump;
		best_score = score;
		}
	}
while (fabs(jump) > err);

return where;
}

/*
	EVALUATE_K1()
	-------------
*/
static double evaluate_k1(double k1, void *param)
{
double k_hat, score, *information_gain = (double *)param;
size_t index;

score = 0;
index = 0;
k_hat = 0;
while (information_gain[index] >= 0)
	{
	score = information_gain[index] / information_gain[1] - ((k1 + 1) * index) / (k1 + index);
	k_hat += score * score;

	index++;
	}

return k_hat;
}

/*
	ANT_RANKING_FUNCTION_BM25ADPT::COMPUTE_K1()
	-------------------------------------------
*/
double ANT_ranking_function_BM25ADPT::compute_k1(ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, double prescalar, double *ig1)
{
static const size_t MAX_TF = 0x100;
long long docid;
double c_prime, tf;
ANT_compressable_integer *current, *end;
ANT_compressable_integer counts[MAX_TF];
double information_gain[MAX_TF];
size_t index, sum;

/*
	Initialise
*/
memset(counts, 0, sizeof(counts));
memset(information_gain, 0, sizeof(information_gain));

/*
	Compute the number of documents containing each c'(t, d) occurences of t
*/
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
sum = 0;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr * prescalar;

	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;

		c_prime =  tf / (1.0 - b + b * ((double)document_lengths[docid] / (double)mean_document_length));
		if ((index = (size_t)ANT_round(c_prime)) < MAX_TF)
			counts[index]++;
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}

/*
	Compute the cumulative sums
*/
sum = 0;
for (index = MAX_TF - 1; index > 0; index--)
	{
	sum += counts[index];
	counts[index] = sum;
	}
counts[0] = documents;
counts[1] = term_details->global_document_frequency;

/*
	Compute the Information Gain, IG
*/
for (index = 0; index < MAX_TF - 1; index++)
	information_gain[index] = -ANT_log2(((double)term_details->global_document_frequency + 0.5) / ((double)documents + 1.0)) + ANT_log2(((double)counts[index + 1] + 0.5) / ((double)counts[index] + 1.0));

/*
	Where to stop
*/
for (index = 3; index < MAX_TF - 1; index++)
	if (information_gain[index] > information_gain[index + 1])
		{
		information_gain[index + 1] = -1;
		break;
		}
information_gain[254] = -1;

*ig1 = information_gain[1];

/*
	find and return the best k1 parameter
*/
return ANT_range_search(10.0, 0.001, evaluate_k1, information_gain);
}

/*
	ANT_RANKING_FUNCTION_BM25ADPT::RELEVANCE_RANK_TOP_K()
	-----------------------------------------------------
*/
void ANT_ranking_function_BM25ADPT::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double c_prime, f_prime, rsv, tf, idf, k1;
ANT_compressable_integer *current, *end;

k1 = compute_k1(term_details, impact_header, impact_ordering, prescalar, &idf);

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

		c_prime = tf / (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length));
		f_prime = ((k1 + 1) * c_prime) / (k1 + c_prime);

		rsv = f_prime * idf;
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

#else
void ANT_ranking_function_BM25ADPT::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
}
#endif

/*
	ANT_RANKING_FUNCTION_BM25ADPT::RANK()
	-------------------------------------
*/
double ANT_ranking_function_BM25ADPT::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
