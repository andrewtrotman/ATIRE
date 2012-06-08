/*
	EVALUATION_BINARY_PREFERENCE.H
	------------------------------
*/
#ifndef EVALUATION_BINARY_PREFERENCE_H_
#define EVALUATION_BINARY_PREFERENCE_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_BINARY_PREFERENCE
	--------------------------------------
*/
class ANT_evaluation_binary_preference : public ANT_evaluation
{
friend class ANT_evaluator;

private:
	double minimum;

public:
	ANT_evaluation_binary_preference() : ANT_evaluation() { this->minimum = 0; }

	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);

	void set_minimum(double minimum) { this->minimum = minimum; }
} ;

#endif /* EVALUATION_BINARY_PREFERENCE_H_ */
