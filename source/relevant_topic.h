/*
	RELEVANT_TOPIC.H
	----------------
*/

#ifndef __RELEVANT_TOPIC_H__
#define __RELEVANT_TOPIC_H__

class ANT_relevant_topic
{
public:
	static int compare(const void *a, const void *b);
public:
	long topic;
	long number_of_relevant_documents;
	long number_of_nonrelevant_documents;
} ;


#endif __RELEVANT_TOPIC_H__
