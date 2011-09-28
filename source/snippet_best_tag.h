/*
	SNIPPET_BEST_TAG.H
	------------------
*/
#ifndef SNIPPET_BEST_TAG_H_
#define SNIPPET_BEST_TAG_H_

#include "snippet.h"

class ANT_search_engine;
class ANT_stem;

/*
	class ANT_SNIPPET_BEST_TAG
	--------------------------
*/
class ANT_snippet_best_tag : public ANT_snippet
{
private:
	char *tag;									// only use content in the first occurence of this tag
	size_t tag_length;							// this is the length of this->tag

public:
	ANT_snippet_best_tag(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag);
	virtual ~ANT_snippet_best_tag() {}

	virtual char *get_snippet(char *snippet, char *document);
} ;

#endif /* SNIPPET_BEST_TAG_H_ */
