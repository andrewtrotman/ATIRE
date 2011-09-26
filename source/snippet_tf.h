/*
	SNIPPET_TF.H
	------------
*/
#ifndef SNIPPET_TF_H_
#define SNIPPET_TF_H_

#include "snippet_tficf.h"

/*
	class ANT_SNIPPET_TF
	--------------------
*/
class ANT_snippet_tf : public ANT_snippet_tficf
{
public:
	ANT_snippet_tf(unsigned long max_length, long length_of_longest_document, ANT_stem *stemmer) : ANT_snippet_tficf(max_length, length_of_longest_document, NULL, stemmer) {}
	virtual ~ANT_snippet_tf() {}
} ;

#endif /* SNIPPET_TF_H_ */
