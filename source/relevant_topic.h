/*
	RELEVANT_TOPIC.H
	----------------
*/

#ifndef RELEVANT_TOPIC_H_
#define RELEVANT_TOPIC_H_

class ANT_relevant_topic
{
public:
	long long topic;
	long long number_of_subtopics;
	long long *subtopics;
	long long *number_of_relevant_documents;
	long long *number_of_nonrelevant_documents;
	long long *number_of_relevant_characters;
	long long *beginning_of_judgements;

public:
	static int compare(const void *a, const void *b);
} ;

#endif  /* RELEVANT_TOPIC_H_ */
