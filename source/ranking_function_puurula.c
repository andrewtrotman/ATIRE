/*
	RANKING_FUNCTION_PUURULA.C
	--------------------------
*/
#include <math.h>
#include "search_engine.h"
#include "pragma.h"
#include "ranking_function_puurula.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

static inline double max(double a, double b) { return a > b ? a : b; }

/*
	ANT_RANKING_FUNCTION_PUURULA::ANT_RANKING_FUNCTION_PUURULA()
	------------------------------------------------------------
*/
ANT_ranking_function_puurula::ANT_ranking_function_puurula(ANT_search_engine *engine, long quantize, long long quantization_bits, double u, double g) : ANT_ranking_function(engine, quantize, quantization_bits)
{
size_t documents, current;
ANT_search_engine_btree_leaf term_details;
unsigned char *postings_buffer;
ANT_compressable_integer *decompress_buffer;
ANT_compression_factory factory;

this->u = u;
this->g = g;

documents = (size_t)engine->document_count();
discounted_document_lengths = new double[documents];

if (engine->get_postings_details("~puurula_length", &term_details) == NULL)
	{
	/*
		The index was constructed without discounted TF values so we do the best we can, which is to use undiscounted lengths
	*/
	for (current = 0; current < documents; current++)
		discounted_document_lengths[current] = document_lengths[current];
	}
else
	{
	postings_buffer = engine->get_postings_buffer();
	decompress_buffer = engine->get_decompress_buffer();

	postings_buffer = engine->get_postings(&term_details, postings_buffer);
	factory.decompress(decompress_buffer, postings_buffer, term_details.local_document_frequency);

	for (current = 0; current < documents; current++)
		discounted_document_lengths[current] = decompress_buffer[current] / 100.0;		// accurate to 2 decimal places
	}
}



#ifdef IMPACT_HEADER

/*
	ANT_RANKING_FUNCTION_PUURULA::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------
*/
void ANT_ranking_function_puurula::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
}

/*
	ANT_RANKING_FUNCTION_PUURULA::DISCOUNT_COLLECTION_FREQUENCY()
	-------------------------------------------------------------
*/
double ANT_ranking_function_puurula::discount_collection_frequency(ANT_impact_header *impact_header)
{
ANT_compressable_integer *doc_count_ptr, *impact_value_ptr;
double tf, discounted_tf, discounted_cf;

discounted_cf = 0;

impact_value_ptr = impact_header->impact_value_start;
doc_count_ptr = impact_header->doc_count_start;
while (doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf =  *impact_value_ptr;
	discounted_tf = max(tf - g * pow(tf, g), 0);

	discounted_cf += *doc_count_ptr * discounted_tf;

	impact_value_ptr++;
	doc_count_ptr++;
	}

return discounted_cf;
}

/*
	ANT_RANKING_FUNCTION_PUURULA::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
	1 parameter is wrong... discounted_document_lengths
*/
void ANT_ranking_function_puurula::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double top_row, tf, idf, discounted_tf, alpha_d, discounted_cf, score;
ANT_compressable_integer *current, *end;

discounted_cf = discount_collection_frequency(impact_header);

impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf =  *impact_header->impact_value_ptr * prescalar;
	discounted_tf = max(tf - g * pow(tf, g), 0);

	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		alpha_d = 1.0 - discounted_document_lengths[(size_t)docid] / ((double)document_lengths[(size_t)docid] + u);
		score = ((1.0 - alpha_d) * (discounted_tf / discounted_cf)) + (alpha_d * ((double)term_details->global_collection_frequency / (double)collection_length_in_terms));

		accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

#else

/*
	ANT_RANKING_FUNCTION_PUURULA::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_puurula::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
}

#endif

/*
	ANT_RANKING_FUNCTION_PUURULA::RANK()
	------------------------------------
*/
double ANT_ranking_function_puurula::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
return term_frequency;
}
