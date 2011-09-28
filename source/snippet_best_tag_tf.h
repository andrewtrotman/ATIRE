/*
	SNIPPET_BEST_TAG_TF.H
	---------------------
*/
#ifndef SNIPPET_BEST_TAG_TF_H_
#define SNIPPET_BEST_TAG_TF_H_

#include "snippet_best_tag.h"

class ANT_search_engine;
class ANT_stem;

/*
	class ANT_SNIPPET_BEST_TAG_TF
	-----------------------------
*/
class ANT_snippet_best_tag_tf : public ANT_snippet_best_tag
{
public:
	ANT_snippet_best_tag_tf(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag) : ANT_snippet_best_tag(max_length, length_of_longest_document, engine, stemmer, tag) { this->engine = NULL; }
	virtual ~ANT_snippet_best_tag_tf() {}
} ;



#endif /* SNIPPET_BEST_TAG_TF_H_ */
