/*
	RANKING_FUNCTION_TFLODOP.C
	--------------------------
	Implementaiton of TDlodop * IDF.  See equation 11 of: F. Rousseau, M. Vazirgiannis (2013) Composition of TF normalizations: new insights on scoring functions for ad hoc IR. Proceedings of SIGIR '13 pp. 917-920
	

	                                tf(t,d)
	TFlodop = 1 + ln (1 + ln (------------------------) + delta)
	                          1 - b _ b * (dl / av_dl)
						 
	Where tf(t,d) is the term count of term t in documnt d, dl is the length of document d, av_dl is the average document length. b and delta are parameters

	then multiply by IDF
				  N+1
	IDF = log (---)
	            n
					
	Where N is the number of documents and n is the number of documents containing the given term.
	
	now TFlodop*IDF =
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "maths.h"
#include "pragma.h"
#include "ranking_function_tflodop.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_TFLODOP::ANT_RANKING_FUNCTION_TFLODOP()
	------------------------------------------------------------
*/
ANT_ranking_function_tflodop::ANT_ranking_function_tflodop(ANT_search_engine *engine, long quantize, long long quantization_bits, double b, double delta) : ANT_ranking_function(engine, quantize, quantization_bits)
{
this->b = b;
this->delta = delta;
printf("using TFlodop with b=%f and delta=%f\n", b, delta);
}

/*
	ANT_RANKING_FUNCTION_TFLODOP::ANT_RANKING_FUNCTION_TFLODOP()
	------------------------------------------------------------
*/
ANT_ranking_function_tflodop::ANT_ranking_function_tflodop(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double b, double delta) : ANT_ranking_function(documents, document_lengths, quantization_bits)
{
this->b = b;
this->delta = delta;
}

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_TFLODOP::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------
*/
void ANT_ranking_function_tflodop::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
exit(printf("Cannot compute ANT_RANKING_FUNCTION_TFLODOP quantum at a time (at the moment)\n"));
}
/*
	ANT_RANKING_FUNCTION_TFLODOP::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_tflodop::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double rsv, tf, idf, tf_part;
ANT_compressable_integer *current, *end;

idf = log(((double)documents + 1) / (double)term_details->global_document_frequency);

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

		tf_part = 1 + log(1 + log(tf / (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length)) + delta));
		rsv = tf_part * idf;

		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

#else
void ANT_ranking_function_tflodop::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
}
#endif

/*
	ANT_RANKING_FUNCTION_TFLODOP::RANK()
	------------------------------------
*/
double ANT_ranking_function_tflodop::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
