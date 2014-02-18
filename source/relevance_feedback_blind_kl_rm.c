/*
	RELEVANCE_FEEDBACK_BLIND_KL_RM.C
	--------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "relevance_feedback_blind_kl_rm.h"
#include "memory_index_one.h"
#include "query.h"
#include "NEXI_term_ant.h"
#include "maths.h"
#include "memory_index_one_node.h"

/*
	ANT_RELEVANCE_FEEDBACK_BLIND_KL_RM::COMPUTE()
	---------------------------------------------
	We're only interested in the original query terms here (we ignore all others).  We also only want the
	posterior probability so we don't need to do much at all.
*/
ANT_memory_index_one_node **ANT_relevance_feedback_blind_kl_rm::compute(long terms_wanted, long *terms_found)
{
long current_term, into, max_terms;
long long length;
ANT_memory_index_one_node **top_terms, *node;

max_terms = ANT_min(query->terms_in_query, terms_wanted);
top_terms = new ANT_memory_index_one_node *[max_terms + 1];
length = one->get_document_length();
for (current_term = into = 0; current_term < max_terms; current_term++)
	if ((node = one->get_term_node(&query->NEXI_query[current_term].term)) != NULL)
		(top_terms[into++] = node)->kl_score = (double)node->term_frequency / (double)length;

*terms_found = into;
top_terms[into] = NULL;

return top_terms;
}

/*
	ANT_RELEVANCE_FEEDBACK_BLIND_KL_RM::FEEDBACK()
	----------------------------------------------
*/
ANT_memory_index_one_node **ANT_relevance_feedback_blind_kl_rm::feedback(ANT_search_engine_result *result, ANT_query *query, long documents_to_examine, long terms_to_fetch, long *terms_found)
{
this->query = query;
    
return  ANT_relevance_feedback_blind_kl::feedback(result, query, documents_to_examine, terms_to_fetch, terms_found);
}
