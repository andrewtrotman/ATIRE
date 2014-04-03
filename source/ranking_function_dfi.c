/*
	RANKING_FUNCTION_DFI.C
	----------------------
	Divergence from Independance model.  See:
	Bekir Taner DinÁer, Ilker Kocabas, Bahar Karaoglan (2010): IRRA at TREC 2010: Index Term Weighting by Divergence From Independence Model. TREC 2010
*/

#include "maths.h"
#include "pragma.h"
#include "ranking_function_dfi.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"


#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DFI::RELEVANCE_RANK_ONE_QUANTUM()
	------------------------------------------------------

	            tf - ef
	rsv = log2(-------- + 1)
	           sqrt(ef)

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
*/
void ANT_ranking_function_DFI::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double cf, tf, ef, rsv;
ANT_compressable_integer *current;

cf = (double)quantum_parameters->term_details->global_collection_frequency;
tf = quantum_parameters->tf * quantum_parameters->prescalar;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;

	ef = cf * ((double)document_lengths[(size_t)docid] / (double)collection_length_in_terms);
	rsv = ANT_log2((tf - ef) / sqrt(ef) + 1);

	if (tf - ef > 0)
		quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_DFI::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
		            tf - ef
	rsv = log2(-------- + 1)
	           sqrt(ef)

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
*/
void ANT_ranking_function_DFI::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double cf, tf, ef, rsv;
ANT_compressable_integer *current, *end;

cf = (double)term_details->global_collection_frequency;

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

		ef = cf * ((double)document_lengths[(size_t)docid] / (double)collection_length_in_terms);
		rsv = ANT_log2((tf - ef) / sqrt(ef) + 1);

		if (tf - ef > 0)
			accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
}
#else

/*
	ANT_RANKING_FUNCTION_DFI::RELEVANCE_RANK_TOP_K()
	------------------------------------------------


	            tf - ef
	rsv = log2(-------- + 1)
	           sqrt(ef)

	where
	          d_len
	ef = cf * -----
	          c_len

	and

	d_len is the document length
	c_len is the collection length
	cf is the collection frequency
	tf is the term frequency
*/
void ANT_ranking_function_DFI::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double tf, cf, ef, score;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
cf = (double)term_details->global_collection_frequency;

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++ * prescalar;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;

		ef = cf * ((double)document_lengths[(size_t)docid] / (double)collection_length_in_terms);
		score = ANT_log2((tf - ef) / sqrt(ef) + 1);

//		if (score > 0)
		if (tf - ef > 0)
			accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DFI::RANK()
	--------------------------------
*/
double ANT_ranking_function_DFI::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
double tf = (double)term_frequency;
double cf = (double)collection_frequency;

double ef = cf * (length / cf);
double score = ANT_log2((tf - ef) / sqrt(ef) + 1);

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
