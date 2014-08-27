/*
	RANKING_FUNCTION_PUURULA.C
	--------------------------
	See 
		A. Puurula. Cumulative Progress in Language Models for Information Retrieval. Australasian Language Technology Workshop, 2013
*/
#include "maths.h"
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

	puts("Estimating the Puurula parameters as this index does not contain them");
	}
else
	{
	postings_buffer = engine->get_postings_buffer();
	decompress_buffer = engine->get_decompress_buffer();

	postings_buffer = engine->get_postings(&term_details, postings_buffer);
	factory.decompress(decompress_buffer, postings_buffer, term_details.local_document_frequency);

	for (current = 0; current < documents; current++)
		discounted_document_lengths[current] = (decompress_buffer[current] - 1) / 1000.0;				// accurate to 3 decimal places
	}
}

#ifdef IMPACT_HEADER

/*
	ANT_RANKING_FUNCTION_PUURULA::RELEVANCE_RANK_ONE_QUANTUM()
	----------------------------------------------------------
*/
void ANT_ranking_function_puurula::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double rsv, tf, cf, query_length, query_occurences, prior;
ANT_compressable_integer *current;

query_length = quantum_parameters->accumulator->get_term_count();
query_occurences = quantum_parameters->query_frequency;

cf = (double)quantum_parameters->term_details->global_collection_frequency;

tf = quantum_parameters->tf;
tf = max(tf - g * pow(tf, g), 0);
																									
rsv = query_occurences * log((tf * quantum_parameters->prescalar * collection_length_in_terms) / (u * cf) + 1.0);

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;

	if (quantum_parameters->accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
		{
 		prior = log(1.0 - discounted_document_lengths[(size_t)docid] / ((double)document_lengths[(size_t)docid] + u));
		quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * (query_length * prior + rsv), maximum_collection_rsv, minimum_collection_rsv));
		}
	else
		quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_PUURULA::RELEVANCE_RANK_TOP_K()
	----------------------------------------------------
*/
void ANT_ranking_function_puurula::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double rsv, tf, cf, query_length, query_occurences, prior;
ANT_compressable_integer *current, *end;

query_length = accumulator->get_term_count();
query_occurences = query_frequency;

cf = (double)term_details->global_collection_frequency;

impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr;
	tf = max(tf - g * pow(tf, g), 0);
																										
//	rsv = query_occurences * log((tf * collection_length_in_terms) / (u * cf) + 1.0);
	rsv = query_occurences * ANT_logsum(log(tf) + log(collection_length_in_terms) - log(u * cf + 1.0), 0);

	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		if (accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
			{
	 		prior = log(1.0 - discounted_document_lengths[(size_t)docid] / ((double)document_lengths[(size_t)docid] + u));
			accumulator->add_rsv(docid, quantize(query_length * prior + rsv, maximum_collection_rsv, minimum_collection_rsv));
			}
		else
			accumulator->add_rsv(docid, quantize(rsv, maximum_collection_rsv, minimum_collection_rsv));
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
void ANT_ranking_function_puurula::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
exit(printf("Cannot use this ranking function with this query processing strategy\n"));
/*
	long long docid;
	double rsv, tf, cf, query_length, query_occurences, prior;
	ANT_compressable_integer *current, *end;

	current = impact_ordering;
	end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);

	query_length = accumulator->get_term_count();
	query_occurences = query_frequency;

	cf = (double)term_details->global_collection_frequency;

	while (current < end)
		{
		end += 2;		// account for the impact_order and the terminator
		tf = *current++;
		tf = max(tf - g * pow(tf, g), 0);

		rsv = query_occurences * log((tf * prescalar * collection_length_in_terms) / (u * cf) + 1.0);

		docid = -1;
		while (*current != 0)
			{
			docid += *current++;

			if (accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
				{
		 		prior = log(1.0 - discounted_document_lengths[(size_t)docid] / ((double)document_lengths[(size_t)docid] + u));
				accumulator->add_rsv(docid, quantize(postscalar * (query_length * prior + rsv), maximum_collection_rsv, minimum_collection_rsv));
				}
			else
				accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
			}
		current++;		// skip over the zero
		}
*/
}
#endif

/*
	ANT_RANKING_FUNCTION_PUURULA::RANK()
	------------------------------------
*/
double ANT_ranking_function_puurula::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
return term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_PUURULA::SCORE_ONE_DOCUMENT()
	--------------------------------------------------
*/
double ANT_ranking_function_puurula::score_one_document(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency, double terms_in_query)
{
double prior, rsv, cf, tf = term_frequency;

tf = max(tf - g * pow(tf, g), 0);
cf = (double)collection_frequency;

rsv = query_frequency * log((tf * collection_length_in_terms) / (u * cf) + 1.0);
prior = log(1.0 - discounted_document_lengths[(size_t)docid] / ((double)document_lengths[(size_t)docid] + u));

return rsv + prior;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
