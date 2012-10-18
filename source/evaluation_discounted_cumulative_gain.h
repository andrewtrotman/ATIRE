/*
	EVALUATION_DISCOUNTED_CUMULATIVE_GAIN.H
	---------------------------------------
*/
#ifndef EVALUATION_DISCOUNTED_CUMULATIVE_GAIN_H_
#define EVALUATION_DISCOUNTED_CUMULATIVE_GAIN_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_DISCOUNTED_CUMULATIVE_GAIN
	-----------------------------------------------
*/
class ANT_evaluation_discounted_cumulative_gain : public ANT_evaluation
{
friend class ANT_evaluator;

protected:
	long trec_mode; // whether to use TREC's version or not, default yes
	double alpha;

public:
	ANT_evaluation_discounted_cumulative_gain() : ANT_evaluation() { this->trec_mode = true; alpha = 0.5; }
	virtual double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);

	void set_mode(long trec_mode) { this->trec_mode = trec_mode; }
	void set_alpha(double alpha) { this->alpha = alpha; }
	
protected:
	double gain(long long relevance, unsigned long long rank);
	double discount(unsigned long long rank);
	static int gain_compare(const void *a, const void *b);

} ;

#endif /* EVALUATION_DISCOUNTED_CUMULATIVE_GAIN_H_ */
