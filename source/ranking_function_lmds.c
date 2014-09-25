/*
	RANKING_FUNCTION_LMDS.C
	----------------------
	Language Models with Dirichlet smoothing

	In:
		M. Petri, J.S. Culpepper, A. Moffat (2013) Exploring the Magic of WAND, Proceedings of ADCS 2013
	A different interpretation of the Language Models with Dirichlet Smoothing is given in which the normalisation
	factor is outside the sumation operator. J. Shane Culpepper wanted an apples-to-apples comparison of this 
	interpretation against BM25, so this is an implementation of that ranking function.
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_lmds.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_LMDS::RELEVANCE_RANK_ONE_QUANTUM()
	------------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmds::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double rsv, idf, query_length, query_occurences;
ANT_compressable_integer *current;

/*
	Each document gets a "prior" of:
	             u
   |q| * log -------
	          |d| + u


	where |q| is the length of the query, |d| is the length of the document, and u is a smoothing parameter (Petri et al. use 2500)
	Then for each term (t in q^d)
	
	                 fdt   |C|
	rsv = fqt * log (--- * --- + 1)
	                  u     Ft

	where fqt is the number of times the term appears in the query, tdt is the number of times the term appears in the document,
	|C| is the length of the collection (measured in terms) and Ft is the number of times t occurs in the collection.
*/
query_length = quantum_parameters->accumulator->get_term_count();
query_occurences = 1.0;		// this is a hack and should be the number of times the term occurs in the query

idf = ((double)collection_length_in_terms / (double)quantum_parameters->term_details->global_collection_frequency);
rsv =	query_occurences * log(((quantum_parameters->tf * quantum_parameters->prescalar )/ u) * idf + 1.0);

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;

	if (quantum_parameters->accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
		quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * (rsv + query_length * log(u / ((double)document_lengths[(size_t)docid] + u))), maximum_collection_rsv, minimum_collection_rsv));
	else
		quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
	}
}

/*
	ANT_RANKING_FUNCTION_LMDS::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmds::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double rsv, tf, idf, query_length, query_occurences;
ANT_compressable_integer *current, *end;

/*
	Each document gets a "prior" of:
	             u
   |q| * log -------
	          |d| + u


	where |q| is the length of the query, |d| is the length of the document, and u is a smoothing parameter (Petri et al. use 2500)
	Then for each term (t in q^d)
	
	                 fdt   |C|
	rsv = fqt * log (--- * --- + 1)
	                  u     Ft

	where fqt is the number of times the term appears in the query, tdt is the number of times the term appears in the document,
	|C| is the length of the collection (measured in terms) and Ft is the number of times t occurs in the collection.
*/
query_length = accumulator->get_term_count();
query_occurences = 1.0;		// this is a hack and should be the number of times the term occurs in the query

idf = ((double)collection_length_in_terms / (double)term_details->global_collection_frequency);

impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr;
	rsv = query_occurences * log(((tf * prescalar) / u) * idf + 1.0);
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		if (accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
			accumulator->add_rsv(docid, quantize(postscalar * (rsv + query_length * log(u / ((double)document_lengths[(size_t)docid] + u))), maximum_collection_rsv, minimum_collection_rsv));
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
	ANT_RANKING_FUNCTION_LMDS::RELEVANCE_RANK_TOP_K()
	------------------------------------------------
	Language Models with Dirichlet smoothing
*/
void ANT_ranking_function_lmds::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
double rsv, tf, idf, query_length, query_occurences;
ANT_compressable_integer *current, *end;

/*
	Each document gets a "prior" of:
	             u
   |q| * log -------
	          |d| + u


	where |q| is the length of the query, |d| is the length of the document, and u is a smoothing parameter (Petri et al. use 2500)
	Then for each term (t in q^d)
	
	                 fdt   |C|
	rsv = fqt * log (--- * --- + 1)
	                  u     Ft

	where fqt is the number of times the term appears in the query, tdt is the number of times the term appears in the document,
	|C| is the length of the collection (measured in terms) and Ft is the number of times t occurs in the collection.
*/

query_length = accumulator->get_term_count();
query_occurences = 1.0;		// this is a hack and should be the number of times the term occurs in the query

current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);

idf = ((double)collection_length_in_terms / (double)term_details->global_collection_frequency);

while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	rsv = query_occurences * log(((tf * prescalar) / u) * idf + 1.0);
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		if (accumulator->is_zero_rsv(docid))		// unseen before now so add the document prior
			accumulator->add_rsv(docid, quantize(postscalar * (rsv + query_length * log(u / ((double)document_lengths[(size_t)docid] + u))), maximum_collection_rsv, minimum_collection_rsv));
		else
			accumulator->add_rsv(docid, quantize(postscalar * rsv, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_LMDS::RANK()
	--------------------------------
*/
double ANT_ranking_function_lmds::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency)
{
exit(printf("Cannot pre-compute the impact score of the LMDS language model as it truely depends on query length and query frequencies"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_LMDS::SCORE_ONE_DOCUMENT()
	-----------------------------------------------
*/
double ANT_ranking_function_lmds::score_one_document(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency, double terms_in_query)
{
double idf, rsv;

idf = (double)collection_length_in_terms / (double)collection_frequency;
rsv =	query_frequency * log((term_frequency/ u) * idf + 1.0);

return rsv + terms_in_query * log(u / ((double)document_lengths[(size_t)docid] + u));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

