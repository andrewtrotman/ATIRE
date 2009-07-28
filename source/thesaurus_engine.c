/*
	THESAURUS_ENGINE.C
	------------------
*/
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "str.h"
#include "ctypes.h"
#include "stemmer.h"
#include "btree_iterator.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "thesaurus_engine.h"
#include "ranking_function.h"

/*
	ANT_THESAURUS_ENGINE::FILL_BUFFER_WITH_POSTINGS()
	-------------------------------------------------
	I want to compare TF values, so I need this to fill a long * buffer with 
	values for terms.
*/
long long ANT_thesaurus_engine::fill_buffer_with_postings(char *term, long *buffer)
{
ANT_search_engine_btree_leaf term_details;
ANT_compressable_integer *current_document, *end;
long document;

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
	document = -1;
	while (*current_document != 0)
		{
		document += *current_document++;
		buffer[document] = term_frequency;
		}
	current_document++;
	}

return term_details.collection_frequency;
}

/*
	ANT_THESAURUS_ENGINE::BUFFER_SIMILARITY()
	-----------------------------------------
  Calculates how similar the two buffers are.
*/
double ANT_thesaurus_engine::buffer_similarity(long long buffer_a_total, long long buffer_b_total) 
{
long long doc, tmp_a = 0, tmp_b = 0;
long long doc_count_a = 0, doc_count_b = 0;
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
	if (buffer_a[doc])
		doc_count_a++;
	if (buffer_b[doc])
		doc_count_b++;
	if (buffer_a[doc] > 0 && buffer_b[doc] == 0)
		tmp_a++;
	if (buffer_b[doc] > 0 && buffer_a[doc] == 0)
		tmp_b++;

	tf_a = (double) buffer_a[doc] / (double) document_lengths[doc];
	tf_b = (double) buffer_b[doc] / (double) document_lengths[doc];

	similarity += tf_a * tf_b;

	length_a += tf_a * tf_a;
	length_b += tf_b * tf_b;
	}

idf_a = log((double) documents / (double) doc_count_a);
idf_b = log((double) documents / (double) doc_count_b);

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

return buffer_similarity(row_total1, row_total2);
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
    Assumes buffer_b is filled with sum of tfs of query terms
*/
double ANT_thesaurus_engine::prob_word_in_query(char **query_terms, int query_term_count, char *word, double p_coll_w, long long documents_returned, double *p_q_d_buffer, double lambda)
{
double prob = 0, mean;
long *doc_lengths;
int i, j;

/*
  P(w|Q) = sigma   P(w|D) * P(D|Q)
          (D in R)

  where R is the set of documents containing at least one of the query terms.
*/
doc_lengths = get_document_lengths(&mean);
fill_buffer_with_postings(word, buffer_a);

for (i = 0; i < documents; i++)
    {
    if (buffer_b[i])
        {
        /*
          P(w|D) = lambda Pml(w|D) + (1 - lambda) Pcoll(w)
          
          Pml = relative frequency
        */
        double pml_w_d = (double)buffer_a[i] / (double) doc_lengths[i];
        double p_w_d = lambda * pml_w_d + (1 - lambda) * p_coll_w;

        /*
          P(D|Q) ~= P(Q|D) P(D) via Bayes. P(Q) is assumed(ish) to be 1.
         */
        double p_d = 1 / (double) documents_returned; // No chance of 0.

        /*
          P(Q|D) =    PI    P(q|D)
                   (q in Q)

          P(q|D) is much like P(w|D)
         */ 

        double p_d_q = p_q_d_buffer[i] * p_d;
        
        prob += p_w_d * p_d_q;
        }
    }

return prob;
}

/*
	ANT_THESAURUS_ENGINE::CLARITY_SCORE()
	-------------------------------------
    Currently uses the Kullback-Leibler divergence between the query and the collection.
    (Jensen-Shannon is another possibility.)
*/
double ANT_thesaurus_engine::clarity_score(char *query, double lambda) 
{
ANT_btree_iterator all_terms(this);
ANT_search_engine_btree_leaf details;
char **query_terms;
char *start, *end, *term;
long long total_collection_frequency = 0, documents_returned = 0;
int i, j, query_term_count = 0;
double score = 0.0;
double *p_q_d_buffer;

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
    query_terms[i] = strndup(start, end - start); /* Allocates memory */
    }

/* Get the total collection frequency - total occurances of all terms */
for (term = all_terms.first("a"); term != NULL; term = all_terms.next()) 
    total_collection_frequency += get_postings_details(term, &details)->collection_frequency;

/* 
   Precalc the P(q|D) for all D
*/
p_q_d_buffer = (double *)malloc(sizeof *p_q_d_buffer * documents);
double mean;
long *doc_lengths = get_document_lengths(&mean);
for (i = 0; i < documents; i++) 
    p_q_d_buffer[j] = 1;

for (i = 0; i < query_term_count; i++) 
    {
    fill_buffer_with_postings(query_terms[i], buffer_a);
    double p_coll_q =
        (double) get_postings_details(query_terms[i], &details)->collection_frequency /
        (double) total_collection_frequency;

    for (j = 0; j < documents; j++) 
        {
        double pml_small_q_d = (double)buffer_a[j] / (double) doc_lengths[j];
        double p_small_q_d = lambda * pml_small_q_d + (1 - lambda) * p_coll_q;
        p_q_d_buffer[j] *= p_small_q_d;
        }
    }

/* 
   We also need buffer_b to have the term totals.
   So we can figure out which docs contain at least one term.
*/
memset(buffer_b, 0, documents * sizeof *buffer_a);
for (i = 0; i < query_term_count; i++)
    {
    fill_buffer_with_postings(query_terms[i], buffer_a);
    for (j = 0; j < documents; j++) 
        buffer_b[j] += buffer_a[j];
    }
for (i = 0; i < documents; i++) 
    if (buffer_b[i])
        documents_returned++;

/* 
   Actual calculation 
                                               P(w|Q)
   Clarity Score =    Sigma      P(w|Q) log_2 ---------
                   (w in Vocab)                Pcoll(w)
   
   Q - query
   Pcoll - Relative frequency in the whole collection
*/

for (term = all_terms.first("a"); term != NULL; term = all_terms.next()) 
    {
    double p_coll_w = 
        (double) get_postings_details(term, &details)->collection_frequency /
        (double) total_collection_frequency;
    double p_w_q = prob_word_in_query(query_terms, query_term_count, term, p_coll_w, documents_returned, p_q_d_buffer, lambda);
    
    score += p_w_q * log(p_w_q / p_coll_w) / log(2);
    }

/* Clean up */
for (i = 0; i < query_term_count; i++)
    {
    free(query_terms[i]);
    }
free(query_terms);
free(p_q_d_buffer);
return score;
}
