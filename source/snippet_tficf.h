/*
	SNIPPET_TFICF.H
	---------------
*/
#ifndef SNIPPET_TFICF_H_
#define SNIPPET_TFICF_H_

#include "snippet.h"

class ANT_search_engine;
class ANT_stem;

/*
	class ANT_SNIPPET_TFICF
	-----------------------
*/
class ANT_snippet_tficf : public ANT_snippet
{
public:
	ANT_snippet_tficf(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer) : ANT_snippet(max_length, length_of_longest_document, engine, stemmer) {}
	virtual ~ANT_snippet_tficf() {}

	virtual char *get_snippet(char *snippet, char *document);
} ;

#endif /* SNIPPET_TFICF_H_ */
