/*
	RELEVANT_DOCUMENT.H
	-------------------
*/

#ifndef RELEVANT_DOCUMENT_H_
#define RELEVANT_DOCUMENT_H_

class ANT_relevant_document
{
public:
	long topic;
	long long docid;
	long relevant_characters;
	long document_length;

public:
	static int compare(const void *a, const void *b);
} ;

#endif  /* RELEVANT_DOCUMENT_H_ */
