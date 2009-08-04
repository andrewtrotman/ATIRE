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
