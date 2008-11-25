/*
	RELEVANT_DOCUMENT.H
	-------------------
*/

#ifndef __RELEVANT_DOCUMENT_H__
#define __RELEVANT_DOCUMENT_H__

class ANT_relevant_document
{
public:
	long topic;
	long docid;
	double rsv;

public:
	static int compare(const void *a, const void *b);
} ;

#endif __RELEVANT_DOCUMENT_H__
