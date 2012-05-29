/*
	EVALUATION.H
	------------
*/
#ifndef EVALUATION_H_
#define EVALUATION_H_

class ANT_memory;
class ANT_relevant_document;
class ANT_relevant_topic;
class ANT_search_engine;

/*
	class ANT_EVALUATION
	--------------------
*/
class ANT_evaluation
{
protected:
	ANT_relevant_document *relevance_list;
	long long relevance_list_length;
	ANT_relevant_topic *relevant_topic_list;
	long long relevant_topic_list_length;
	unsigned long long precision_point;

public:
	ANT_evaluation() {}
	virtual ~ANT_evaluation() {}

	void set_precision_point(unsigned long long precision_point) { this->precision_point = precision_point; }
	virtual void set_lists(ANT_relevant_document *relevance_list, long long relevance_list_length, ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length);
	ANT_relevant_topic *setup(long long topic);
	virtual double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);
} ;

#endif /* EVALUATION_H_ */
