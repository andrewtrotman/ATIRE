/*
	EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK.H
	--------------------------------------------------
*/
#ifndef EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK_H_
#define EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK_H_

/*
	class ANT_EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK
	----------------------------------------------------------
*/
class ANT_evaluation_intent_aware_expected_reciprocal_rank : public ANT_evaluation_expected_reciprocal_rank
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);
} ;

#endif /* EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK_H_ */
