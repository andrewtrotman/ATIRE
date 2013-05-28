/*
	RANKING_FUNCTION_DLH13.C
	------------------------
	See:
	C. Macdonald, I. Ounis (2006) Voting for candidates: adapting data fusion techniques for an expert search task. In Proceedings of CIKM 2006. pp.387-396
*/
#ifdef _MSC_VER
	/*
		This is necessary for Microsoft C/C++ to include M_PI
	*/
	#define _USE_MATH_DEFINES
#endif
#include "maths.h"
#include "pragma.h"
#include "ranking_function_dlh13.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"


#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION_DLH13::RELEVANCE_RANK_ONE_QUANTUM()
	--------------------------------------------------------
*/
void ANT_ranking_function_DLH13::relevance_rank_one_quantum(ANT_ranking_function_quantum_parameters *quantum_parameters)
{
long long docid;
double tf, cf, score;
ANT_compressable_integer *current;

cf = (double)quantum_parameters->term_details->global_collection_frequency;
tf = quantum_parameters->tf;

docid = -1;
current = quantum_parameters->the_quantum;
while (current < quantum_parameters->quantum_end)
	{
	docid += *current++;
	// this is DLH13:
	//		score = (1.0 / (tf + 0.5)) * (ANT_log2(((tf * mean_document_length) / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[(size_t)docid]))));
	// this is what Terrier actually uses:
	score = 1.0 * (quantum_parameters->tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[(size_t)docid])))) / (tf + 0.5);

	// In both cases you get negative numbers and so we add a bit (this is a hack)
	score += 10.0;

	quantum_parameters->accumulator->add_rsv(docid, quantize(quantum_parameters->postscalar * score, maximum_collection_rsv, minimum_collection_rsv));

	}
}

/*
	ANT_RANKING_FUNCTION_DLH13::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
*/
void ANT_ranking_function_DLH13::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, cf, score;
ANT_compressable_integer *current, *end;

/*
						qtw             tf * avg_l   N                                   tf
	rsv = ---------- * (log2(---------- * -) + 0.5 * log2 (2 * pi * tf * (1 - ---)))
					tf + 0.5            l          F                                    l

	where
		qtw = qft / qtf_max
	and
		qtf     = number of times the term occurs in the query (usually 1)
		qtf_max = maximum number of times a unique terms occurs in the query (usually 1)
		tf      = term frequency
		avg_l   = average document length from collection
		l       = length of document (in tokens)
		N       = number of documents in the collection
		F       = collection frequency
		pi      = pi (3.141...)
*/
cf = (double)term_details->global_collection_frequency;
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while (impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	tf = *impact_header->impact_value_ptr;
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		// this is DLH13:
		//		score = (1.0 / (tf + 0.5)) * (ANT_log2(((tf * mean_document_length) / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[(size_t)docid]))));
		// this is what Terrier actually uses:
		score = 1.0 * (tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[(size_t)docid])))) / (tf + 0.5);

		// In both cases you get negative numbers and so we add a bit (this is a hack)
		score += 10.0;

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
	ANT_RANKING_FUNCTION_DLH13::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
*/
void ANT_ranking_function_DLH13::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long long docid;
double tf, cf, score;
ANT_compressable_integer *current, *end;

/*
            qtw             tf * avg_l   N                                   tf
	rsv = ---------- * (log2(---------- * -) + 0.5 * log2 (2 * pi * tf * (1 - ---)))
          tf + 0.5            l          F                                    l

	where
		qtw = qft / qtf_max
	and
		qtf     = number of times the term occurs in the query (usually 1)
		qtf_max = maximum number of times a unique terms occurs in the query (usually 1)
		tf      = term frequency
		avg_l   = average document length from collection
		l       = length of document (in tokens)
		N       = number of documents in the collection
		F       = collection frequency
		pi      = pi (3.141...)
*/

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

// this is DLH13:
//		score = (1.0 / (tf + 0.5)) * (ANT_log2(((tf * mean_document_length) / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[(size_t)docid]))));
// this is what Terrier actually uses:
		score = 1.0 * (tf * ANT_log2((tf * mean_document_length / document_lengths[(size_t)docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[(size_t)docid])))) / (tf + 0.5);

// In both cases you get negative numbers and so we add a bit (this is a hack)
		score += 10.0;

		accumulator->add_rsv(docid, quantize(postscalar * score, maximum_collection_rsv, minimum_collection_rsv));
		}
	current++;		// skip over the zero
	}
}
#endif

/*
	ANT_RANKING_FUNCTION_DLH13::RANK()
	---------------------------------
*/
double ANT_ranking_function_DLH13::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency)
{
double cf, tf, score;

tf = (double)term_frequency;
cf = (double)collection_frequency;

// this is DLH13:
//score = (1.0 / (tf + 0.5)) * (ANT_log2(((tf * mean_document_length) / document_lengths[docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[docid]))));
// this is what Terrier actually uses:
score = 1.0 * (tf * ANT_log2((tf * mean_document_length / document_lengths[docid]) * (documents / cf)) + 0.5 * ANT_log2(2.0 * M_PI * tf * (1.0 - (tf / document_lengths[docid])))) / (tf + 0.5);
// In both cases you get negative numbers and so we add a bit (this is a hack)
score += 10.0;

return score;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
