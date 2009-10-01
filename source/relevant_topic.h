/*
	RELEVANT_TOPIC.H
	----------------
*/

#ifndef RELEVANT_TOPIC_H_
#define RELEVANT_TOPIC_H_

class ANT_relevant_topic
{
public:
	static int compare(const void *a, const void *b);
public:
	long topic;
	long number_of_relevant_documents;
	long number_of_nonrelevant_documents;
} ;


#endif  /* RELEVANT_TOPIC_H_ */
