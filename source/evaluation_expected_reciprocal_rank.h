/*
	EVALUATION_EXPECTED_RECIPROCAL_RANK.H
	-------------------------------------
*/
#ifndef EVALUATION_EXPECTED_RECIPROCAL_RANK_H_
#define EVALUATION_EXPECTED_RECIPROCAL_RANK_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_EXPECTED_RECIPROCAL_RANK
	---------------------------------------------
*/
class ANT_evaluation_expected_reciprocal_rank : public ANT_evaluation
{
friend class ANT_evaluator;

private:
	double maximum_judgement; // the maximum grade a judgement can have, TREC's gdeval sets this to 4, so we use that as a default
	
public:
	ANT_evaluation_expected_reciprocal_rank() : ANT_evaluation() { this->maximum_judgement = 4; }
	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);
	void set_maximum_judgement(double maximum_judgement) { this->maximum_judgement = maximum_judgement; }

} ;

#endif /* EVALUATION_EXPECTED_RECIPROCAL_RANK_H_ */
