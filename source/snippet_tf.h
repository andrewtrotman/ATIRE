/*
	SNIPPET_TF.H
	------------
*/
#ifndef SNIPPET_TF_H_
#define SNIPPET_TF_H_

#include "snippet_tfidf.h"

/*
	class ANT_SNIPPET_TF
	--------------------
*/
class ANT_snippet_tf : public ANT_snippet_tfidf
{
public:
	ANT_snippet_tf(unsigned long max_length, long length_of_longest_document) : ANT_snippet_tfidf(max_length, length_of_longest_document, NULL) {}
	virtual ~ANT_snippet_tf() {}
} ;

#endif /* SNIPPET_TF_H_ */
