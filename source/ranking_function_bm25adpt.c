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
exit(printf("Not implemented yet"));
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
	EVALUATE_K1()
	-------------
*/
static double evaluate_k1(double k1, void *param)
{
double g, sum = *(double *)param;

if (k1 == 1)
	g = 1;
else
	g = (k1 / (k1 - 1)) * log(k1);

return (g - sum) * (g - sum);
}


/*
	ANT_RANKING_FUNCTION_BM25ADPT::COMPUTE_K1()
	-------------------------------------------
*/
double ANT_ranking_function_BM25ADPT::compute_k1(ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, double prescalar)
{
long long docid;
double c_prime, tf, sum;
ANT_compressable_integer *current, *end;

/*
	Compute the part dependant on the postings lists.
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

		c_prime =  tf / (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length));
		sum += log(c_prime + 1);
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}

sum /= term_details->global_document_frequency;

return ANT_secant(1.0, 1.1, evaluate_k1, &sum);
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

k1 = compute_k1(term_details, impact_header, impact_ordering, prescalar);

idf = log((double)documents / (double)term_details->global_document_frequency);

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
