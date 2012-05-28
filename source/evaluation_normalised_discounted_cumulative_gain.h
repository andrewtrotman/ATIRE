/*
	EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN.H
	--------------------------------------------------
*/
#ifndef EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN_H_
#define EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN_H_

#include "evaluation.h"

/*
	class ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN
	----------------------------------------------------------
*/
class ANT_evaluation_normalised_discounted_cumulative_gain : public ANT_evaluation
{
private:
	long trec_mode; // whether to use TREC's version or not, default yes
	double **ideal_gains;

public:
	ANT_evaluation_normalised_discounted_cumulative_gain() : ANT_evaluation() { this->trec_mode = true; ideal_gains = NULL; }
	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);

	void set_mode(long trec_mode) { this->trec_mode = trec_mode; }
	
	void set_lists(ANT_relevant_document *relevance_list, long long relevance_list_length, ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length);

private:
	static int gain_compare(const void *a, const void *b);

} ;

#endif /* EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN_H_ */
