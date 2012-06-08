/*
	EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN.H
	--------------------------------------------------
*/
#ifndef EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN_H_
#define EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN_H_

/*
	class ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN
	----------------------------------------------------------
*/
class ANT_evaluation_normalised_discounted_cumulative_gain : public ANT_evaluation_discounted_cumulative_gain
{
private:
	double *ideal_gains;

public:
	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);

	void set_lists(ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length);

} ;

#endif /* EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN_H_ */
