/*
	RANKING_FUNCTION_PUURULA_IDF.C
	------------------------------
	See 
		A. Puurula. Cumulative Progress in Language Models for Information Retrieval. Australasian Language Technology Workshop, 2013
*/
#include <math.h>
#include "search_engine.h"
#include "pragma.h"
#include "ranking_function_puurula_idf.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

static inline double max(double a, double b) { return a > b ? a : b; }

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::ANT_RANKING_FUNCTION_PUURULA_IDF()
	--------------------------------------------------------------------
*/
ANT_ranking_function_puurula_idf::ANT_ranking_function_puurula_idf(ANT_search_engine *engine, long quantize, long long quantization_bits, double u, double g) : ANT_ranking_function(engine, quantize, quantization_bits)
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
	ANT_RANKING_FUNCTION_PUURULA_IDF::RELEVANCE_RANK_ONE_QUANTUM()
	--------------------------------------------------------------
*/
void ANT_ranking_function_puurula_idf::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
}

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------------
*/
void ANT_ranking_function_puurula_idf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double rsv, tf, df, query_length, query_occurences, prior;
ANT_compressable_integer *current, *end;

query_length = accumulator->get_term_count();
query_occurences = 1.0;		// this is a hack and should be the number of times the term occurs in the query (which is almost always 1 anyway)

df = 1.0 / (double)collection_length_in_terms;		// should this be the number of unique terms in the collections?

impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;

		tf = *impact_header->impact_value_ptr;
		tf = log(1.0 + tf / document_lengths[docid]) * log((double)documents / (double)term_details->global_document_frequency);		// should use unique words in document not document_lengths[]
		tf = max(tf - g * pow(tf, g), 0);

		rsv = query_occurences * log((tf * prescalar) / (u * df) + 1.0);

		if (accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
			{
			prior = query_length * log(1.0 - discounted_document_lengths[(size_t)docid] / ((double)document_lengths[(size_t)docid] + u));
			accumulator->add_rsv(docid, quantize(postscalar * (rsv + prior), maximum_collection_rsv, minimum_collection_rsv));
			}
		else
			accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}


#else

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------------
*/
void ANT_ranking_function_puurula_idf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
}

#endif

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::RANK()
	----------------------------------------
*/
double ANT_ranking_function_puurula_idf::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
