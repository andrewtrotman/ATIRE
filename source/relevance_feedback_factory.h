/*
	RELEVANCE_FEEDBACK_FACTORY.H
	----------------------------
*/
#ifndef RELEVANCE_FEEDBACK_FACTORY_H_
#define RELEVANCE_FEEDBACK_FACTORY_H_

class ANT_search_engine;
class ANT_relevance_feedback;

/*
	class ANT_RELEVANCE_FEEDBACK_FACTORY
	------------------------------------
*/
class ANT_relevance_feedback_factory
{
public:
	enum { NONE, BLIND_KL, TOPSIG, BLIND_RM };

public:
	ANT_relevance_feedback_factory() {}
	virtual ~ANT_relevance_feedback_factory() {}

	ANT_relevance_feedback *get_feedbacker(ANT_search_engine *search_engine, long long which);
} ;

#endif /* RELEVANCE_FEEDBACK_FACTORY_H_ */
