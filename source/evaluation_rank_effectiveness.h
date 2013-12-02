/*
	EVALUATION_RANK_EFFECTIVENESS.H
	-------------------------------
*/
#ifndef EVALUATION_RANK_EFFECTIVENESS_H_
#define EVALUATION_RANK_EFFECTIVENESS_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_RANK_EFFECTIVENESS
	---------------------------------------
*/
class ANT_evaluation_rank_effectiveness : public ANT_evaluation
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic = 0);
} ;

#endif /* EVALUATION_RANK_EFFECTIVENESS_H_ */
