/*
	EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION.H
	------------------------------------------------
*/
#ifndef EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION_H_
#define EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION_H_

/*
	class ANT_EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION
	--------------------------------------------------------
*/
class ANT_evaluation_intent_aware_mean_average_precision : public ANT_evaluation_mean_average_precision
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic = 0);
} ;

#endif /* EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION_H_ */
