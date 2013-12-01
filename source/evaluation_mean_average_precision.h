/*
	EVALUATION_MEAN_AVERAGE_PRECISION.H
	-----------------------------------
*/
#ifndef EVALUATION_MEAN_AVERAGE_PRECISION_H_
#define EVALUATION_MEAN_AVERAGE_PRECISION_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_MEAN_AVERAGE_PRECISION
	-------------------------------------------
*/
class ANT_evaluation_mean_average_precision : public ANT_evaluation
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic = 0);
} ;

#endif /* EVALUATION_MEAN_AVERAGE_PRECISION_H_ */
