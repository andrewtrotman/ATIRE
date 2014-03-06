/*
	RANKING_FUNCTION_BM25L.C
	------------------------
	Implementaiton of BM25L.  See: Y. Lv, CX. Zhai (2011) When Documents Are Very Long, BM25 Fails!, Proceedgins of SIGIR11, pp 1103-1104.


	Lv & Zhai take the BM25 function and alter the TF component.  They re-write it as

	         (k1 + 1) * c'(q,D)
	f(q,D) = ------------------
					k1 + c'(q,D)
		
	Where c'(q,D) is the adjusted verison of the TF part.  Specifically they define c'(q,D) as:
	
	                c(q,D)
	c'(q,D) = -----------------------
	          1 - b + b * (dl / av_dl)
				 
	where c(q,D) is the term count of term q in document D.  b and k1 are parameters. dl is the document length and av_dl is the average document length

	Now the the first equation by adding a small delta thus:
	
	           | (k1 + 1) * c'(q,D) + delta
	           | --------------------------      if c'(q,D) > 0
	           |    k1 + (c'(q,D) + delta
	f'(q,D) = <
	           |
	           | 0                               otherwise
	           |


	BM25L is, therefore IDF(q) * f'(q,D)
	
	Where
	
	              N
	IDF(q) = log ----
	             n(q)
					 
	where N is the number of documents in the collection and nq is the number of documents containing term q

*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_bm25l.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BM25L::ANT_RANKING_FUNCTION_BM25L()
	--------------------------------------------------------
*/
ANT_ranking_function_BM25L::ANT_ranking_function_BM25L(ANT_search_engine *engine, long quantize, long long quantization_bits, double k1, double b, double delta) : ANT_ranking_function(engine, quantize, quantization_bits)
{
this->k1 = k1;
this->b = b;
this->delta = delta;
}

/*
	ANT_RANKING_FUNCTION_BM25L::ANT_RANKING_FUNCTION_BM25L()
	--------------------------------------------------------
*/
ANT_ranking_function_BM25L::ANT_ranking_function_BM25L(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double k1, double b, double delta) : ANT_ranking_function(documents, document_lengths, quantization_bits)
{
this->k1 = k1;
this->b = b;
this->delta = delta;
}

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_BM25L::RELEVANCE_RANK_ONE_QUANTUM()
	--------------------------------------------------------
*/
void ANT_ranking_function_BM25L::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
double tf, idf, rsv, c_prime, f_prime;
long long docid;
ANT_compressable_integer *current;

idf = log((double)documents / (double)quantum_parameters->term_details->global_document_frequency);
tf = quantum_parameters->tf * quantum_parameters->prescalar;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;

	if ((c_prime =  tf / (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length))) > 0)
		{
		f_prime = ((k1 + 1) * (c_prime + delta)) / (k1 + c_prime + delta);

		rsv = f_prime * idf;
		quantum_parameters->accumulator->add_rsv(docid, quantize(rsv * quantum_parameters->postscalar, maximum_collection_rsv, minimum_collection_rsv));
		}
	}
}

/*
	ANT_RANKING_FUNCTION_BM25L::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------
*/
void ANT_ranking_function_BM25L::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double c_prime, f_prime, rsv, tf, idf;
ANT_compressable_integer *current, *end;

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

		if ((c_prime =  tf / (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length))) > 0)
			{
			f_prime = ((k1 + 1) * (c_prime + delta)) / (k1 + c_prime + delta);

			rsv = f_prime * idf;
			accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
			}
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

#else
void ANT_ranking_function_BM25L::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
}
#endif

/*
	ANT_RANKING_FUNCTION_BM25L::RANK()
	----------------------------------
*/
double ANT_ranking_function_BM25L::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
