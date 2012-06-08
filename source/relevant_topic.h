/*
	RELEVANT_TOPIC.H
	----------------
*/

#ifndef RELEVANT_TOPIC_H_
#define RELEVANT_TOPIC_H_

class ANT_relevant_subtopic;

class ANT_relevant_topic
{
public:
	long long topic;
	
	ANT_relevant_subtopic *subtopic_list;
	long long number_of_subtopics;
	double ideal_gain;

public:
	static int compare(const void *a, const void *b);
} ;

#endif  /* RELEVANT_TOPIC_H_ */
