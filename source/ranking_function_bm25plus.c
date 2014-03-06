/*
	RANKING_FUNCTION_BM25PLUS.C
	---------------------------
	Implementaiton of BM25+.  See: Y. Lv, CX. Zhai (2011) Lower-Bounding Term Frequency Normalization, Proceedings of CIKM'11, pp. 7-16

	OK, so, like its BM25, except that you add a delta (typically 1) to the TF component

	            (k1 + 1) * c(t,D)								N
	w = (--------------------------------------  + delta) * log(-)
	     k1 * (1 - b + b * (dl/av_dl)) + c(t,D)                 n

	That's is. Since delta (apparently) should normally equal 1, the paper is basically an "add one" paper!
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_bm25plus.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BM25PLUS::ANT_RANKING_FUNCTION_BM25PLUS()
	--------------------------------------------------------------
*/
ANT_ranking_function_BM25PLUS::ANT_ranking_function_BM25PLUS(ANT_search_engine *engine, long quantize, long long quantization_bits, double k1, double b, double delta) : ANT_ranking_function(engine, quantize, quantization_bits)
{
this->k1 = k1;
this->b = b;
this->delta = delta;
}

/*
	ANT_RANKING_FUNCTION_BM25PLUS::ANT_RANKING_FUNCTION_BM25PLUS()
	--------------------------------------------------------------
*/
ANT_ranking_function_BM25PLUS::ANT_ranking_function_BM25PLUS(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits, double k1, double b, double delta) : ANT_ranking_function(documents, document_lengths, quantization_bits)
{
this->k1 = k1;
this->b = b;
this->delta = delta;
}

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_BM25PLUS::RELEVANCE_RANK_ONE_QUANTUM()
	-----------------------------------------------------------
*/
void ANT_ranking_function_BM25PLUS::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
double tf, idf, rsv;
long long docid;
ANT_compressable_integer *current;

idf = log((double)documents / (double)quantum_parameters->term_details->global_document_frequency);
tf = quantum_parameters->tf * quantum_parameters->prescalar;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;

	rsv = idf * (((k1 + 1) * tf) / (k1 * (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length)) + tf) + delta);
	quantum_parameters->accumulator->add_rsv(docid, quantize(rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_BM25PLUS::RELEVANCE_RANK_TOP_K()
	-----------------------------------------------------
*/
void ANT_ranking_function_BM25PLUS::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double rsv, tf, idf;
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

		rsv = idf * (((k1 + 1) * tf) / (k1 * (1 - b + b * ((double)document_lengths[docid] / (double)mean_document_length)) + tf) + delta);
		accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

#else

void ANT_ranking_function_BM25PLUS::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
}
#endif

/*
	ANT_RANKING_FUNCTION_BM25PLUS::RANK()
	-------------------------------------
*/
double ANT_ranking_function_BM25PLUS::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
