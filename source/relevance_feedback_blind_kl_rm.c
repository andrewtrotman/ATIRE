/*
	RELEVANCE_FEEDBACK_BLIND_KL.C
	-----------------------------
*/
#include "relevance_feedback_blind_kl.h"
#include "memory_index_one.h"
#include "term_divergence_kl.h"

/*
	ANT_RELEVANCE_FEEDBACK_BLIND_KL::COMPUTE()
	------------------------------------------
*/
ANT_memory_index_one_node **ANT_relevance_feedback_blind_kl::compute(long terms_wanted, long *terms_found)
{
ANT_term_divergence_kl divergence;

one->kl_divergence(&divergence, search_engine);
return one->top_n_terms(terms_wanted, terms_found);
}

/*
	ANT_RELEVANCE_FEEDBACK_BLIND_KL::FEEDBACK()
	-------------------------------------------
*/
ANT_memory_index_one_node **ANT_relevance_feedback_blind_kl::feedback(ANT_search_engine_result *result, long documents_to_examine, long terms_to_fetch, long *terms_found)
{
populate(result, documents_to_examine);
return compute(terms_to_fetch, terms_found);
}

/*
	ANT_RELEVANCE_FEEDBACK_BLIND_KL::FEEDBACK()
	-------------------------------------------
*/
ANT_memory_index_one_node **ANT_relevance_feedback_blind_kl::feedback(char *document, long terms_wanted, long *terms_found)
{
populate(document);
return compute(terms_wanted, terms_found);
}


