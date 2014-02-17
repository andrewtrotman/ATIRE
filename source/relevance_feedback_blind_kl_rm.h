/*
	RELEVANCE_FEEDBACK_BLIND_KL.H
	-----------------------------
*/
#ifndef RELEVANCE_FEEDBACK_BLIND_KL_H_
#define RELEVANCE_FEEDBACK_BLIND_KL_H_

#include "relevance_feedback.h"
/*
	class ANT_RELEVANCE_FEEDBACK_BLIND_KL
	-------------------------------------
*/
class ANT_relevance_feedback_blind_kl : public ANT_relevance_feedback
{
private:
	ANT_memory_index_one_node **compute(long terms_wanted, long *terms_found);

public:
	ANT_relevance_feedback_blind_kl(ANT_search_engine *engine) : ANT_relevance_feedback(engine) {}
	virtual ~ANT_relevance_feedback_blind_kl() {}

	virtual ANT_memory_index_one_node **feedback(ANT_search_engine_result *result, long documents_to_examine, long terms_wanted, long *terms_found);
	virtual ANT_memory_index_one_node **feedback(char *document, long terms_wanted, long *terms_found);
} ;

#endif /* RELEVANCE_FEEDBACK_BLIND_KL_H_ */

