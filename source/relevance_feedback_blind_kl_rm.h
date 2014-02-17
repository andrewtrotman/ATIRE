/*
	RELEVANCE_FEEDBACK_BLIND_KL_RM.H
	--------------------------------
*/
#ifndef RELEVANCE_FEEDBACK_BLIND_KL_RM_H_
#define RELEVANCE_FEEDBACK_BLIND_KL_RM_H_

#include "relevance_feedback_blind_kl.h"

class ANT_query;

/*
	class ANT_RELEVANCE_FEEDBACK_BLIND_KL_RM
	----------------------------------------
*/
class ANT_relevance_feedback_blind_kl_rm : public ANT_relevance_feedback_blind_kl
{
private:
	ANT_query *query;

private:
	virtual ANT_memory_index_one_node **compute(long terms_wanted, long *terms_found);

public:
	ANT_relevance_feedback_blind_kl_rm(ANT_search_engine *engine) : ANT_relevance_feedback_blind_kl(engine) {}
	virtual ~ANT_relevance_feedback_blind_kl_rm() {}

	virtual ANT_memory_index_one_node **feedback(ANT_search_engine_result *result, ANT_query *query, long documents_to_examine, long terms_wanted, long *terms_found);
} ;

#endif /* RELEVANCE_FEEDBACK_BLIND_KL_RM_H_ */

