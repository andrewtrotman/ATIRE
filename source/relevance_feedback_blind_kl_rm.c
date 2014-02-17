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

/*
	ANT_RELEVANCE_FEEDBACK_BLIND_KL_RM::COMPUTE()
	---------------------------------------------
	We're only interested in the original query terms here (we ignore all others).  We also only want the
	posterior probability so we don't need to do much at all.
*/
ANT_memory_index_one_node **ANT_relevance_feedback_blind_kl_rm::compute(long terms_wanted, long *terms_found)
{
long current_term;

for (current_term = 0; current_term < query->terms_in_query; current_term++)
	printf("%*.*s ", (int)query->NEXI_query[current_term].term.length(), (int)query->NEXI_query[current_term].term.length(), query->NEXI_query[current_term].term.string());

exit(puts("Not Yet.... still working on it"));

return NULL;
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
