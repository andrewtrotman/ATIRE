/*
	EVALUATION_BPREF.H
	------------------
*/
#ifndef EVALUATION_BPREF_H_
#define EVALUATION_BPREF_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_BPREF
	--------------------------
*/
class ANT_evaluation_bpref : public ANT_evaluation
{
private:
	long minimum;

public:
	ANT_evaluation_bpref() : ANT_evaluation() { this->minimum = 0; }

	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);

	void set_minimum(long minimum) { this->minimum = minimum; }
} ;

#endif /* EVALUATION_BPREF_H_ */
