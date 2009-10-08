/*
	RANKING_FUNCTION.C
	------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "ranking_function.h"
#include "search_engine_stats.h"

/*
	ANT_RANKING_FUNCTION::ANT_RANKING_FUNCTION()
	--------------------------------------------
*/
ANT_ranking_function::ANT_ranking_function(ANT_search_engine *engine)
{
documents_as_integer = engine->document_count();
documents = (double)documents_as_integer;

collection_length_in_terms_as_integer = engine->get_collection_length();
collection_length_in_terms = (double)collection_length_in_terms_as_integer;

document_lengths = engine->get_document_lengths(&mean_document_length);
decompress_buffer = engine->get_decompress_buffer();

stats = engine->get_stats();
}

/*
	ANT_RANKING_FUNCTION::ANT_RANKING_FUNCTION()
	--------------------------------------------
*/
ANT_ranking_function::ANT_ranking_function(long long documents, ANT_compressable_integer *document_lengths)
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
}

/*
	ANT_RANKING_FUNCTION::TF_TO_POSTINGS()
	--------------------------------------
	convert from an array of tf values into an ANT postings list that can then be used
	in a top-k ranking function
*/
void ANT_ranking_function::tf_to_postings(ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *destination, ANT_weighted_tf *stem_buffer)
{
ANT_compressable_integer bucket_prev_docid[0x100];
long bucket_size[0x100];
ANT_compressable_integer sum, *pointer[0x100];
long doc, document_frequency, bucket, buckets_used;
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
document_frequency = 0;
for (current = stem_buffer; current < end; current++)
	if (*current != 0)					// the stemmed term frequency accumulator list contains zeros.
		{	
		if (*current >= 0x100)
			*current = 0xFF;			// cap term frequency at 255
		bucket_size[(long) *current]++;
		document_frequency++;						// count the document frequency
		}

/*
	Compute the location of the pointers for each bucket
*/
buckets_used = sum = 0;
for (bucket = 0xFF; bucket >= 0; bucket--)
	{
	pointer[bucket] = destination + sum + 2 * buckets_used;
	sum += bucket_size[bucket];
	if (bucket_size[bucket] != 0)
		{
		*pointer[bucket]++ = bucket;
		buckets_used++;
		}
	}

/*
	Now generate the impact ordering
*/
for (current = stem_buffer; current < end; current++)
	if (*current != 0)
		{
		doc = current - stem_buffer + 1;
		*pointer[(long)*current]++ = doc - bucket_prev_docid[(long)*current];		// because this list is difference encoded
		bucket_prev_docid[(long)*current] = doc;
		}

/*
	Finally terminate each impact list with a 0
*/
for (bucket = 0; bucket < 0x100; bucket++)
	if (bucket_size[bucket] != 0)
		*pointer[bucket] = 0;

term_details->document_frequency = document_frequency;
term_details->impacted_length = sum + 2 * buckets_used;
}

/*
	ANT_RANKING_FUNCTION::RELEVANCE_RANK_TF()
	-----------------------------------------
*/
void ANT_ranking_function::relevance_rank_tf(ANT_search_engine_accumulator *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array, long long trim_point)
{
long long now;
now = stats->start_timer();
tf_to_postings(term_details, decompress_buffer, tf_array);
stats->add_stemming_reencode_time(stats->stop_timer(now));

relevance_rank_top_k(accumulator, term_details, decompress_buffer, trim_point);
}

/*
	ANT_RANKING_FUNCTION::COMPUTE_TERM_DETAILS()
	--------------------------------------------
*/
void ANT_ranking_function::compute_term_details(ANT_search_engine_btree_leaf *term_details, ANT_weighted_tf *tf_array)
{
long document_frequency;
ANT_weighted_tf *current, *end;
long long now;

/*
	Compute document frequency which is needed for many ranking functions (but not all)
*/
now = stats->start_timer();
end = tf_array + documents_as_integer;
document_frequency = 0;

for (current = tf_array; current < end; current++)
    if (*current != 0)					// the stemmed term frequency accumulator list contains zeros.
		document_frequency++;

term_details->document_frequency = document_frequency;
term_details->impacted_length = 0;
stats->add_stemming_reencode_time(stats->stop_timer(now));
}
