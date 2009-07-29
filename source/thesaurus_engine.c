/*
	THESAURUS_ENGINE.C
	------------------
*/
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "str.h"
#include "ctypes.h"
#include "stemmer.h"
#include "btree_iterator.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"
#include "thesaurus_engine.h"
#include "ranking_function.h"

/*
	ANT_THESAURUS_ENGINE::FILL_BUFFER_WITH_POSTINGS()
	-------------------------------------------------
	I want to compare TF values, so I need this to fill a long * buffer with 
	values for terms.

    Returns |tf| * |tf| since I need this for base_term, but can't have it, as I'm doing random access.
*/
long long ANT_thesaurus_engine::fill_buffer_with_postings(char *term, long *buffer)
{
ANT_search_engine_btree_leaf term_details;
ANT_compressable_integer *current_document, *end;
long document;
long long tf_length_squared = 0;
ANT_compressable_integer term_frequency;

memset(buffer, 0, (size_t)(sizeof (*buffer) * documents));

// Get position 
if (get_postings_details(term, &term_details) == NULL) 
	return 0;

// Get from disk
if (get_postings(&term_details, postings_buffer) == NULL) 
	return 0;

 // Decompress
factory.decompress(decompress_buffer, postings_buffer, term_details.impacted_length);
current_document = decompress_buffer;
end = decompress_buffer + term_details.impacted_length;
while (current_document < end)
	{
	term_frequency = *current_document++;
	tf_length_squared = term_frequency * term_frequency;
	document = -1;
	while (*current_document != 0)
		{
		document += *current_document++;
		buffer[document] = term_frequency;
		tf_length_squared += term_frequency * term_frequency;
		}
	current_document++;
	}

return tf_length_squared;
}

/*
	ANT_THESAURUS_ENGINE::BUFFER_SIMILARITY()
	-----------------------------------------
  Calculates how similar the two buffers are.
*/
double ANT_thesaurus_engine::buffer_similarity(char *a, long long buffer_a_total, char *b, long long buffer_b_total) 
{
ANT_search_engine_btree_leaf details;
long long doc;
double length_a = 0, length_b = 0;
double similarity = 0;
double tf_a, tf_b, idf_a, idf_b;

if (buffer_a_total == 0 || buffer_b_total == 0)
	return 0.0;

for (doc = 0; doc < documents; doc++)
	{
	/* 
	            A . B 
	cos theta = -------
	           |A| |B|

	where A and B are tf.idf scores.
	we can open up the expressions and multiply by 

	idf_a * idf_b  OR idf_a ^ 2 OR idf_b ^ 2

	at the end.


	TF*IDF;
	TF = term_count / |document|
             
	IDF = log(|collection| / doc_count)

	*/
	tf_a = (double) buffer_a[doc] / (double) document_lengths[doc];
	tf_b = (double) buffer_b[doc] / (double) document_lengths[doc];

	similarity += tf_a * tf_b;

    // This portion required for all docs with tf scores.
	length_a += tf_a * tf_a;
	length_b += tf_b * tf_b;
	}


idf_a = log((double) documents / (double) get_postings_details(a, &details)->document_frequency);
idf_b = log((double) documents / (double) get_postings_details(b, &details)->document_frequency);

similarity *= idf_a * idf_b;

similarity /= sqrt((double)length_a * idf_a * idf_a);
similarity /= sqrt((double)length_b * idf_b * idf_b);

return similarity;
}

/* 
	ANT_THESAURUS_ENGINE::TERM_SIMILARITY()
	---------------------------------------

   This is in effect a matrix multiplication, between the index and its 
   transpose. Each postings list must be normalised, and the multiplication
   happens such that the matrix left is term x term.

            doc a, doc b             term A, term B              term A, term B
   term A [     0      1 ] \/  doc a [    0       1 ]  == term A [    1       0 ]
   term B [     1      0 ] /\  doc b [    1       0 ]  == term B [    0       1 ]

   (obviously we do this in another way for efficiency)
*/
double ANT_thesaurus_engine::term_similarity(char *term1, char *term2)
{
long long row_total1, row_total2;

row_total1 = fill_buffer_with_postings(term1, buffer_a);
row_total2 = fill_buffer_with_postings(term2, buffer_b);

return buffer_similarity(term1, row_total1, term2, row_total2);
}

/*
	ANT_THESAURUS_ENGINE::STEMMING_EXCEPTIONS()
	-------------------------------------------
   Stemming exceptions - print these out for now, we can do something with them later.
*/
void ANT_thesaurus_engine::stemming_exceptions(ANT_stemmer *stemmer, double threshold)
{
ANT_btree_iterator all_terms(this); 
double similarity;
char *term;
char *stem;

// .first("a") skips all the numbers and tags
// alternately check that the first char is alpha via ANT_isalpha(term[0]);
for (term = all_terms.first("a"); term != NULL; term = all_terms.next())
	for (stem = stemmer->first(term); stem != NULL; stem = stemmer->next())
		{
		/* Prevent doing everything twice (turn the resultant matrix into a triangle) */
		if (strcmp(term, stem) == 0)
			break;

		similarity = term_similarity(term, stem);
		if (similarity != 0.0)
			printf("%s -- %s [label=%f]\n", term, stem, similarity);
		else
			printf("%s; %s;\n", term, stem); // ensure the nodes show up
		}
}

/*
	ANT_THESAURUS_ENGINE::PROCESS_ONE_STEMMED_SEARCH_TERM()
	-------------------------------------------------------
*/
void ANT_thesaurus_engine::process_one_stemmed_search_term(ANT_stemmer *stemmer, char *base_term, ANT_ranking_function *ranking_function)
{
ANT_search_engine_btree_leaf term_details, stemmed_term_details;
long long collection_frequency;
ANT_compressable_integer *current_document, *end;
long document;
char *term;
ANT_compressable_integer term_frequency;

memset(stem_buffer, 0, (size_t)stem_buffer_length_in_bytes);
collection_frequency = 0;

term = stemmer->first(base_term);

while (term != NULL)
	{
    // TODO: Fill buffer with base_term first.
	if (strcmp(base_term, term) == 0 || term_similarity(base_term, term) > threshold)
		{
		stemmer->get_postings_details(&term_details);

		if (get_postings(&term_details, postings_buffer) == NULL)
			return;

		factory.decompress(decompress_buffer, postings_buffer, term_details.impacted_length);

		collection_frequency += term_details.collection_frequency;

		current_document = decompress_buffer;
		end = decompress_buffer + term_details.impacted_length;
		while (current_document < end)
			{
			term_frequency = *current_document++;
			document = -1;
			while (*current_document != 0)
				{
				document += *current_document++;
				stem_buffer[document] += term_frequency;
				}
			current_document++;
			}
		}
	term = stemmer->next();
	}

stemmed_term_details.collection_frequency = collection_frequency;
ranking_function->relevance_rank_tf(accumulator, &stemmed_term_details, stem_buffer, trim_postings_k);
}

/*
	ANT_THESAURUS_ENGINE::PROB_WORD_IN_QUERY()
	------------------------------------------
    P(w|Q) = sum(for D in documents) SCORE(w) SCORE(Q)

    where w is a word in the query ( P(w|Q) is zero otherwise)
    SCORE(w) is some ranking score given the word w.
    SCORE(Q) is some ranking score given the entire query.
*/
double ANT_thesaurus_engine::prob_word_in_query(char *query_term, ANT_search_engine_accumulator::ANT_accumulator_t *full_query_results, ANT_ranking_function *ranking_function)
{
double prob = 0;
long long i, hits;

/* 
   Cache the results list for the entire query.
*/
init_accumulators();
process_one_search_term(query_term, ranking_function);
sort_results_list(LLONG_MAX, &hits); 

for (i = 0; i < documents; i++)
    {
    if (accumulator_pointers[i]->is_zero_rsv())
        break;              // Done!

    prob += 
        ((double) accumulator_pointers[i]->get_rsv() / 100) *
        ((double) full_query_results[accumulator_pointers[i] - accumulator] / 100);
    }

return prob;
}

/*
	ANT_THESAURUS_ENGINE::CLARITY_SCORE()
	-------------------------------------
    Currently uses the Kullback-Leibler divergence between the query and the collection.
    (Jensen-Shannon is another possibility.)
*/
double ANT_thesaurus_engine::clarity_score(char *query, ANT_ranking_function *ranking_function)
{
ANT_search_engine_btree_leaf details;
long long hits;
char **query_terms;
char *start, *end;
long i, query_term_count = 0;
double score = 0.0;
ANT_search_engine_accumulator *ranked_list;
ANT_search_engine_accumulator::ANT_accumulator_t *cached_results;

/*
  Get the query terms.
*/
end = query;
while (*end != '\0') 
    {
    start = end;
    while (!ANT_isalpha(*start))
        start++;
    end = start + 1;
    while (ANT_isalpha(*end))
        end++;
    query_term_count++;
    }

query_terms = (char **)malloc(sizeof *query_terms * query_term_count);

end = query;
for (i = 0; i < query_term_count; i++) 
    {
    start = end;
    while (!ANT_isalpha(*start)) 
        start++;
    end = start + 1;
    while (ANT_isalpha(*end))
        end++;
    query_terms[i] = strnnew(start, end - start); /* Allocates memory */
    }

/* 
   Cache the results list for the entire query.
*/
init_accumulators();
for (i = 0; i < query_term_count; i++) 
    process_one_search_term(query_terms[i], ranking_function);
sort_results_list(LLONG_MAX, &hits); 

cached_results = (ANT_search_engine_accumulator::ANT_accumulator_t *)malloc(sizeof *cached_results * documents);
memset(cached_results, 0, sizeof *cached_results * documents);
for (i = 0; i < documents; i++)
    cached_results[accumulator_pointers[i] - accumulator] = accumulator_pointers[i]->get_rsv();

/* 
   Actual calculation 
                                               P(w|Q)
   Clarity Score =    Sigma      P(w|Q) log_2 ---------
                   (w in Vocab)                Pcoll(w)
   
   RHS is 0 whenever w isn't in Q (as P(w|Q) == Pcoll(w) if w !in Q, which makes log_2(1) which is 0)
   Q - query
   Pcoll - Relative frequency in the whole collection
*/

for (i = 0; i < query_term_count; i++) 
    {
    double p_coll_w = 0;
    if (get_postings_details(query_terms[i], &details))
        p_coll_w = (double) details.collection_frequency /
            (double) collection_length_in_terms;
    else
        continue;               // Avoid / by 0
    double p_w_q = prob_word_in_query(query_terms[i], cached_results, ranking_function);
    
    score += p_w_q * log(p_w_q / p_coll_w) / log(2.0);
    }

/* Clean up */
for (i = 0; i < query_term_count; i++)
    delete [] query_terms[i];

free(query_terms);
free(cached_results);
return log(score);
}
