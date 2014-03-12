/*
	RANKING_FUNCTION.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pragma.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"
#include "ranking_function.h"
#include "stats_search_engine.h"
#include "bitstring.h"

/*
	ANT_RANKING_FUNCTION::ANT_RANKING_FUNCTION()
	--------------------------------------------
*/
ANT_ranking_function::ANT_ranking_function(ANT_search_engine *engine, long quantize, long long quantization_bits)
{
documents_as_integer = engine->document_count();
documents = (double)documents_as_integer;

this->quantization = quantize;
/*
	See: Crane M, Trotman A, and O'Keefe R. Maintaining Discriminatory Power in Quantized Indexes
*/
this->quantization_bits = (long long)(quantization_bits == -1 ? 5.4 + 5.4e-4 * sqrt(documents) : quantization_bits);

long long dummy;
minimum_collection_rsv = *(double *)&(dummy = engine->get_variable("~quantmin"));
maximum_collection_rsv = *(double *)&(dummy = engine->get_variable("~quantmax"));

collection_length_in_terms_as_integer = engine->get_collection_length();
collection_length_in_terms = (double)collection_length_in_terms_as_integer;

document_lengths = engine->get_document_lengths(&mean_document_length);
#ifdef IMPACT_HEADER
the_impact_header = engine->get_impact_header();
#endif
decompress_buffer = engine->get_decompress_buffer();

stats = engine->get_stats();
}

/*
	ANT_RANKING_FUNCTION::ANT_RANKING_FUNCTION()
	--------------------------------------------
*/
ANT_ranking_function::ANT_ranking_function(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits)
{
ANT_compressable_integer *current, *end;

decompress_buffer = NULL;
engine = NULL;
stats = NULL;

documents_as_integer = documents;
this->documents = (double)documents;

collection_length_in_terms_as_integer = 0;
end = document_lengths + documents;
for (current = document_lengths; current < end; current++)
	collection_length_in_terms_as_integer += *current;
collection_length_in_terms = (double)collection_length_in_terms_as_integer;
mean_document_length = collection_length_in_terms / documents;

this->document_lengths = document_lengths;

decompress_buffer = NULL;

stats = NULL;

// because this is only created when we do index quantization, we can guarantee this
quantization = true;
this->quantization_bits = quantization_bits;
}

#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION::RELEVANCE_RANK_QUANTUM()
	----------------------------------------------
	pass every single quantum to the rank function for evaluation
*/
void ANT_ranking_function::relevance_rank_quantum(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
ANT_compressable_integer *current, *end;
ANT_ranking_function_quantum_parameters quantum_parameters;

quantum_parameters.accumulator = accumulator;
quantum_parameters.term_details = term_details;
quantum_parameters.prescalar = prescalar;
quantum_parameters.postscalar = postscalar;
quantum_parameters.query_frequency = query_frequency;

impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while(impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	quantum_parameters.tf = *impact_header->impact_value_ptr;
	end = current + *impact_header->doc_count_ptr;
	quantum_parameters.the_quantum = current;
	quantum_parameters.quantum_end = end;
	relevance_rank_one_quantum(&quantum_parameters);
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#endif

/*
	ANT_RANKING_FUNCTION::TF_TO_POSTINGS()
	--------------------------------------
	convert from an array of tf values into an ANT postings list that can then be used
	in a top-k ranking function
*/
#ifdef IMPACT_HEADER
void ANT_ranking_function::tf_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *destination, ANT_impact_header *impact_header, ANT_weighted_tf *stem_buffer)
{
long bucket, buckets_used;
ANT_compressable_integer bucket_prev_docid[0x100];
long long bucket_size[0x100];
ANT_compressable_integer doc, *pointer[0x100];
long long document_frequency, collection_frequency, sum;
ANT_weighted_tf *current, *end;

/*
	Set all the buckets to empty;
*/
memset(bucket_size, 0, sizeof(bucket_size));

/*
	Set the previous document ID to zero for each bucket (for difference encoding)
*/
memset(bucket_prev_docid, 0, sizeof(bucket_prev_docid));

/*
	Compute the size of the buckets - and as we are stemming we also have to cap Term Frequency at 255.
*/
end = stem_buffer + documents_as_integer;
document_frequency = collection_frequency = 0;
for (current = stem_buffer; current < end; current++)
	if (*current != 0)					// the stemmed term frequency accumulator list contains zeros.
		{
		if (*current >= 0x100)
			*current = 0xFF;			// cap term frequency at 255
		bucket_size[(size_t) *current]++;
		document_frequency++;						// count the document frequency
		collection_frequency += *current;			// count the collecton frequency
		}

// find the number of non-zero buckets (the number of quantums)
impact_header->the_quantum_count = 0;
for (bucket = 0; bucket < 0x100; bucket++)
	if (bucket_size[bucket] != 0)
		impact_header->the_quantum_count++;

// setup the pointers for the header
impact_header->impact_value_ptr = impact_header->impact_value_start = impact_header->header_buffer;
impact_header->doc_count_ptr = impact_header->doc_count_start = impact_header->header_buffer + impact_header->the_quantum_count;
impact_header->impact_offset_ptr = impact_header->impact_offset_start = impact_header->header_buffer + impact_header->the_quantum_count * 2;
impact_header->doc_count_trim_ptr = impact_header->impact_offset_start;

/*
	Compute the location of the pointers for each bucket
*/
sum = buckets_used = 0;
for (bucket = 0xFF; bucket >= 0; bucket--)
	{
	//pointer[bucket] = destination + sum + 2 * buckets_used;
	pointer[bucket] = destination + sum;
	if (bucket_size[(size_t)bucket] != 0)
		{
		//*pointer[(size_t)bucket]++ = bucket;
		*impact_header->impact_value_ptr = bucket;
		impact_header->impact_value_ptr++;
		*impact_header->doc_count_ptr = (ANT_compressable_integer)bucket_size[bucket];
		impact_header->doc_count_ptr++;
		*impact_header->impact_offset_ptr = (ANT_compressable_integer)sum;
		impact_header->impact_offset_ptr++;
		buckets_used++;
		}
	sum += bucket_size[(size_t)bucket];
	}

/*
	Now generate the impact ordering
*/
for (current = stem_buffer; current < end; current++)
	if (*current != 0)
		{
		doc = (ANT_compressable_integer)(current - stem_buffer + 1);
		*pointer[(size_t)*current]++ = doc - bucket_prev_docid[(size_t)*current];		// because this list is difference encoded
		bucket_prev_docid[(size_t)*current] = doc;
		}

/*
	Finally terminate each impact list with a 0
*/
//for (bucket = 0; bucket < 0x100; bucket++)
//	if (bucket_size[bucket] != 0)
//		*pointer[bucket] = 0;

term_details->local_document_frequency = term_details->global_document_frequency = document_frequency;
term_details->local_collection_frequency = term_details->global_collection_frequency = collection_frequency;
term_details->impacted_length = sum;
}
#else
void ANT_ranking_function::tf_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *destination, ANT_weighted_tf *stem_buffer)
{
long bucket, buckets_used;
ANT_compressable_integer bucket_prev_docid[0x100];
long long bucket_size[0x100];
ANT_compressable_integer doc, *pointer[0x100];
long long document_frequency, collection_frequency, sum;
ANT_weighted_tf *current, *end;

/*
	Set all the buckets to empty;
*/
memset(bucket_size, 0, sizeof(bucket_size));

/*
	Set the previous document ID to zero for each bucket (for difference encoding)
*/
memset(bucket_prev_docid, 0, sizeof(bucket_prev_docid));

/*
	Compute the size of the buckets - and as we are stemming we also have to cap Term Frequency at 255.
*/
end = stem_buffer + documents_as_integer;
document_frequency = collection_frequency = 0;
for (current = stem_buffer; current < end; current++)
	if (*current != 0)					// the stemmed term frequency accumulator list contains zeros.
		{
		if (*current >= 0x100)
			*current = 0xFF;			// cap term frequency at 255
		bucket_size[(size_t) *current]++;
		document_frequency++;						// count the document frequency
		collection_frequency += *current;			// count the collecton frequency
		}

/*
	Compute the location of the pointers for each bucket
*/
sum = buckets_used = 0;
for (bucket = 0xFF; bucket >= 0; bucket--)
	{
	pointer[bucket] = destination + sum + 2 * buckets_used;
	sum += bucket_size[(size_t)bucket];
	if (bucket_size[(size_t)bucket] != 0)
		{
		*pointer[(size_t)bucket]++ = bucket;
		buckets_used++;
		}
	}

/*
	Now generate the impact ordering
*/
for (current = stem_buffer; current < end; current++)
	if (*current != 0)
		{
		doc = (ANT_compressable_integer)(current - stem_buffer + 1);
		*pointer[(size_t)*current]++ = doc - bucket_prev_docid[(size_t)*current];		// because this list is difference encoded
		bucket_prev_docid[(size_t)*current] = doc;
		}

/*
	Finally terminate each impact list with a 0
*/
for (bucket = 0; bucket < 0x100; bucket++)
	if (bucket_size[bucket] != 0)
		*pointer[bucket] = 0;

term_details->local_document_frequency = term_details->global_document_frequency = document_frequency;
term_details->local_collection_frequency = term_details->global_collection_frequency = collection_frequency;
term_details->impacted_length = sum + 2 * buckets_used;
}
#endif


#ifdef IMPACT_HEADER
/*
	ANT_RANKING_FUNCTION::RELEVANCE_RANK_BOOLEAN()
	----------------------------------------------
*/
void ANT_ranking_function::relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_impact_header *impact_header, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
ANT_compressable_integer *current, *end;

/*
	Set the bits in the bitstring (pass 1)
*/
impact_header->impact_value_ptr = impact_header->impact_value_start;
impact_header->doc_count_ptr = impact_header->doc_count_start;
current = impact_ordering;
while(impact_header->doc_count_ptr < impact_header->doc_count_trim_ptr)
	{
	docid = -1;
	end = current + *impact_header->doc_count_ptr;
	while (current < end)
		{
		docid += *current++;
		documents_touched->unsafe_setbit(docid);
		}
	current = end;
	impact_header->impact_value_ptr++;
	impact_header->doc_count_ptr++;
	}

/*
	Now call the top-k based relevance ranking function (pass 2)
*/
relevance_rank_quantum(accumulators, term_details, impact_header, impact_ordering, trim_point, prescalar, postscalar, query_frequency);
//relevance_rank_top_k(accumulators, term_details, impact_header, impact_ordering, trim_point, prescalar, postscalar, query_frequency);

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
#else
/*
	ANT_RANKING_FUNCTION::RELEVANCE_RANK_BOOLEAN()
	----------------------------------------------
*/
void ANT_ranking_function::relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long docid;
ANT_compressable_integer *current, *end;

/*
	Set the bits in the bitstring (pass 1)
*/
current = impact_ordering;
end = impact_ordering + (term_details->local_document_frequency >= trim_point ? trim_point : term_details->local_document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	current++;		// skip over the TF value
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		documents_touched->unsafe_setbit(docid);
		}
	current++;		// skip over the zero
	}

/*
	Now call the top-k based relevance ranking function (pass 2)
*/
relevance_rank_top_k(accumulators, term_details, impact_ordering, trim_point, prescalar, postscalar, query_frequency);
}
#endif

/*
	ANT_RANKING_FUNCTION::RELEVANCE_RANK_TF()
	-----------------------------------------
*/
void ANT_ranking_function::relevance_rank_tf(ANT_bitstring *bitstring, ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array, long long trim_point, double prescalar, double postscalar, double query_frequency)
{
long long now;
now = stats->start_timer();

#ifdef IMPACT_HEADER
	tf_to_postings(term_details, decompress_buffer, the_impact_header, tf_array);
#else
	tf_to_postings(term_details, decompress_buffer, tf_array);
#endif
stats->add_stemming_reencode_time(stats->stop_timer(now));

if (bitstring == NULL)
	{
	#ifdef IMPACT_HEADER
		//relevance_rank_quantum(accumulator, term_details, the_impact_header, decompress_buffer, trim_point, prescalar, postscalar, query_frequency);
		relevance_rank_top_k(accumulator, term_details, the_impact_header, decompress_buffer, trim_point, prescalar, postscalar, query_frequency);
	#else
		relevance_rank_top_k(accumulator, term_details, decompress_buffer, trim_point, prescalar, postscalar, query_frequency);
	#endif
	}
else
	{
	#ifdef IMPACT_HEADER
		relevance_rank_boolean(bitstring, accumulator, term_details, the_impact_header, decompress_buffer, trim_point, prescalar, postscalar, query_frequency);
	#else
		relevance_rank_boolean(bitstring, accumulator, term_details, decompress_buffer, trim_point, prescalar, postscalar, query_frequency);
	#endif
	}
}

/*
	ANT_RANKING_FUNCTION::COMPUTE_TERM_DETAILS()
	--------------------------------------------
*/
#ifdef IMPACT_HEADER
void ANT_ranking_function::compute_term_details(ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array)
{
long long document_frequency, collection_frequency;
ANT_weighted_tf *current, *end;
long long now;

/*
	Compute document frequency which is needed for many ranking functions (but not all)
*/
now = stats->start_timer();
document_frequency = collection_frequency = 0;
the_impact_header->impact_value_ptr = the_impact_header->impact_value_start;
the_impact_header->doc_count_ptr = the_impact_header->doc_count_start;
current = tf_array;
// impact_offset_start is the end of the doc_count
while(the_impact_header->doc_count_ptr < the_impact_header->impact_offset_start) {
	end = current + *the_impact_header->doc_count_ptr;
	while (current < end) {
		collection_frequency += *current;
		document_frequency++;
	}
	current = end;
	the_impact_header->impact_value_ptr++;
	the_impact_header->doc_count_ptr++;
}

term_details->local_document_frequency = term_details->global_document_frequency = document_frequency;
term_details->local_collection_frequency = term_details->global_collection_frequency = collection_frequency;
term_details->impacted_length = 0;
stats->add_stemming_reencode_time(stats->stop_timer(now));
}
#else
void ANT_ranking_function::compute_term_details(ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array)
{
long long document_frequency, collection_frequency;
ANT_weighted_tf *current, *end;
long long now;

/*
	Compute document frequency which is needed for many ranking functions (but not all)
*/
now = stats->start_timer();
end = tf_array + documents_as_integer;
document_frequency = collection_frequency = 0;

for (current = tf_array; current < end; current++)
    if (*current != 0)					// the stemmed term frequency accumulator list contains zeros.
		{
		collection_frequency += *current;
		document_frequency++;
		}

term_details->local_document_frequency = term_details->global_document_frequency = document_frequency;
term_details->local_collection_frequency = term_details->global_collection_frequency = collection_frequency;
term_details->impacted_length = 0;
stats->add_stemming_reencode_time(stats->stop_timer(now));
}
#endif

/*
	ANT_RANKING_FUNCTION::GET_MAX_MIN()
	-----------------------------------
	maximum and minimum have already been initialised
*/
void ANT_ranking_function::get_max_min(double *maximum, double *minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned short *term_frequencies)
{
long docid;
double rsv;
unsigned short *current_tf, *end;
ANT_compressable_integer *current_docid;

current_docid = document_ids;
current_tf = term_frequencies;
end = term_frequencies + document_frequency;

docid = -1;
while (current_tf < end)
	{
	docid += *current_docid;
	rsv = rank(docid, document_lengths[docid], *current_tf, collection_frequency, document_frequency, 1);

	if (rsv > *maximum)
		*maximum = rsv;
	if (rsv < *minimum)
		*minimum = rsv;

	current_tf++;
	current_docid++;
	}
}

/*
	ANT_RANKING_FUNCTION::QUANTIZE()
	--------------------------------
*/
void ANT_ranking_function::quantize(double maximum, double minimum, long long collection_frequency, long long document_frequency, ANT_compressable_integer *document_ids, unsigned short *term_frequencies)
{
long docid;
unsigned short *current_tf, *end;
ANT_compressable_integer *current_docid;

current_docid = document_ids;
current_tf = term_frequencies;
end = term_frequencies + document_frequency;

docid = -1;
while (current_tf < end)
	{
	docid += *current_docid;
	*current_tf = (unsigned short)quantize(rank(docid, document_lengths[docid], *current_tf, collection_frequency, document_frequency, 1), maximum, minimum);
	current_tf++;
	current_docid++;
	}
}

/*
	ANT_RANKING_FUNCTION::QUANTIZE()
	--------------------------------
*/
double ANT_ranking_function::quantize(double rsv, double maximum, double minimum)
{
/*
	Quantizes the rsv into the range minimum -> maximum
	8 bits -> 1 << 8 = 256, but we want 1-255, so -1 to get to right range, -1 to avoid 0
*/
return quantization ? (((rsv - minimum) / (maximum - minimum)) * ((1 << quantization_bits) - 2)) + 1 : rsv;
}

/*
	ANT_RANKING_FUNCTION::SCORE_ONE_DOCUMENT()
	------------------------------------------
*/
double ANT_ranking_function::score_one_document(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned short term_frequency, long long collection_frequency, long long document_frequency, double query_frequency, double terms_in_query)
{
return 1.0;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
