/*
	EVALUATION.H
	------------
*/
#ifndef EVALUATION_H_
#define EVALUATION_H_

class ANT_memory;
class ANT_relevant_topic;
class ANT_relevant_subtopic;
class ANT_search_engine;

/*
	class ANT_EVALUATION
	--------------------
*/
class ANT_evaluation
{
protected:
	ANT_relevant_topic *relevant_topic_list;
	long long relevant_topic_list_length;
	long long precision_point;

public:
	ANT_evaluation() {}
	virtual ~ANT_evaluation() {}

	void set_precision_point(long long precision_point) { this->precision_point = precision_point; }
	virtual void set_lists(ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length);
	ANT_relevant_subtopic *setup(long long topic, long long subtopic);
	virtual double evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic = 0);
} ;

#endif /* EVALUATION_H_ */
