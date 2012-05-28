/*
	EVALUATION_PRECISION_AT_N.H
	---------------------------
*/
#ifndef EVALUATION_PRECISION_AT_N_H_
#define EVALUATION_PRECISION_AT_N_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_PRECISION_AT_N
	-----------------------------------
*/
class ANT_evaluation_precision_at_n : public ANT_evaluation
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);
} ;

#endif /* EVALUATION_PRECISION_AT_N_H_ */
