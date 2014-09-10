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
#include "memory_index.h"

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

printf("u=%f g=%f\n", u, g);

documents = (size_t)engine->document_count();
discounted_document_lengths = new double[documents];
tfidf_discounted_document_lengths = new double[documents];
unique_terms_in_document = new double[documents];

postings_buffer = engine->get_postings_buffer();
decompress_buffer = engine->get_decompress_buffer();

unique_terms_in_collection = (double)engine->get_variable("~uniqueterms");

if ((int)(g * 1000) > 9)
	exit(printf("g is out of range in the LMPTFIDF ranking function (check your command line parameters"));

if (engine->get_postings_details(ANT_memory_indexer::squiggle_puurula_tfidf_powerlaw_length[(int)(g * 1000)]->string(), &term_details) == NULL)
	exit(printf("Must be indexed with Puurula length vectors (-Ilmptfidf) to use this ranking function"));

puts(ANT_memory_indexer::squiggle_puurula_tfidf_powerlaw_length[(int)(g * 1000)]->string());

postings_buffer = engine->get_postings(&term_details, postings_buffer);
factory.decompress(decompress_buffer, postings_buffer, term_details.local_document_frequency);
for (current = 0; current < documents; current++)
	discounted_document_lengths[current] = (decompress_buffer[current] - 1) / 1000.0;		// accurate to 3 decimal places


engine->get_postings_details("~puurula_tfidf_length", &term_details);
postings_buffer = engine->get_postings(&term_details, postings_buffer);
factory.decompress(decompress_buffer, postings_buffer, term_details.local_document_frequency);
for (current = 0; current < documents; current++)
	tfidf_discounted_document_lengths[current] = (decompress_buffer[current] - 1) / 1000.0;	// accurate to 3 decimal places


engine->get_postings_details("~unique_terms_in_document", &term_details);
postings_buffer = engine->get_postings(&term_details, postings_buffer);
factory.decompress(decompress_buffer, postings_buffer, term_details.local_document_frequency);
for (current = 0; current < documents; current++)
	unique_terms_in_document[current] = decompress_buffer[current] - 1;
}

#ifdef IMPACT_HEADER

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::RELEVANCE_RANK_ONE_QUANTUM()
	--------------------------------------------------------------
*/
void ANT_ranking_function_puurula_idf::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
exit(printf("Cannot use lmptfidf and quantum processing (at the moment)\n"));
}

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::RELEVANCE_RANK_TOP_K()
	--------------------------------------------------------
*/
void ANT_ranking_function_puurula_idf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double rsv, tf, query_length, query_occurences, prior;
ANT_compressable_integer *current, *end;

query_length = accumulator->get_term_count();
query_occurences = query_frequency;				// this has already been transformed by the TFxIDF equation.

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
	 	tf = log(1.0 + tf / unique_terms_in_document[docid]) * log((double)documents / (double)term_details->global_document_frequency);	// L0 norm version
	 	tf = max(tf - g * pow(tf, g), 0);

		if (tf != 0)
			{
			/*
				There must be some TF component to the score or else the rsv == 0 which is equivelant to the term not occuring
			*/
	 		rsv = query_occurences * log((tf * unique_terms_in_collection) / u + 1.0);
			if (accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
				{
				prior = log(1.0 - discounted_document_lengths[(size_t)docid] / (tfidf_discounted_document_lengths[(size_t)docid] + u));
				accumulator->add_rsv(docid, quantize(query_length * prior + rsv, maximum_collection_rsv, minimum_collection_rsv));
				}
			else													// seen so we have already added the prior
				accumulator->add_rsv(docid, quantize(rsv, maximum_collection_rsv, minimum_collection_rsv));
			}
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
void ANT_ranking_function_puurula_idf::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
}

#endif

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::RANK()
	----------------------------------------
*/
double ANT_ranking_function_puurula_idf::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_PUURULA_IDF::SCORE_ONE_DOCUMENT()
	------------------------------------------------------
*/
double ANT_ranking_function_puurula_idf::score_one_document(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency, double terms_in_query)
{
double prior, rsv, tf = term_frequency;

tf = log(1.0 + tf / unique_terms_in_document[docid]) * log((double)documents / (double)document_frequency);
tf = max(tf - g * pow(tf, g), 0);

rsv = query_frequency * log((tf * (double)unique_terms_in_collection) / u + 1.0);

prior = log(1.0 - discounted_document_lengths[(size_t)docid] / ((double)tfidf_discounted_document_lengths[(size_t)docid] + u));

return rsv + prior;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
