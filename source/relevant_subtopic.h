/*
	RELEVANT_SUBTOPIC.H
	-------------------
*/

#ifndef RELEVANT_SUBTOPIC_H_
#define RELEVANT_SUBTOPIC_H_

class ANT_relevant_document;

class ANT_relevant_subtopic
{
public:
	long long subtopic;
	long long topic;
	
	ANT_relevant_document *document_list;
	long long number_of_documents;

	long long number_of_relevant_documents;
	long long number_of_nonrelevant_documents;
	long long number_of_relevant_characters;

public:
	static int compare(const void *a, const void *b);
} ;

#endif  /* RELEVANT_SUBTOPIC_H_ */
