/*
	EVALUATION_INTENT_AWARE_PRECISION_AT_N.H
	----------------------------------------
*/
#ifndef EVALUATION_INTENT_AWARE_PRECISION_AT_N_H_
#define EVALUATION_INTENT_AWARE_PRECISION_AT_N_H_

/*
	class ANT_EVALUATION_INTENT_AWARE_PRECISION_AT_N
	------------------------------------------------
*/
class ANT_evaluation_intent_aware_precision_at_n : public ANT_evaluation_precision_at_n
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic = 0);
} ;

#endif /* EVALUATION_INTENT_AWARE_PRECISION_AT_N_H_ */
