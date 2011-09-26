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
protected:
	ANT_search_engine *engine;						// the search engine associated with the documents (for the purpose of global term statistics)
	ANT_stem *stemmer;								// any stemming that should apply to terms in the document

public:
	ANT_snippet_tficf(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer);
	virtual ~ANT_snippet_tficf() {}

	virtual char *get_snippet(char *snippet, char *document, char *query);
} ;


#endif /* SNIPPET_TFICF_H_ */
