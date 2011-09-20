/*
	SNIPPET_TFIDF.H
	---------------
*/
#ifndef SNIPPET_TFIDF_H_
#define SNIPPET_TFIDF_H_

#include "snippet.h"

class ANT_search_engine;

/*
	class ANT_SNIPPET_TFIDF
	-----------------------
*/
class ANT_snippet_tfidf : public ANT_snippet
{
protected:
	ANT_search_engine *engine;

public:
	ANT_snippet_tfidf(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine);
	virtual ~ANT_snippet_tfidf() {}

	virtual char *get_snippet(char *snippet, char *document, char *query);
} ;



#endif /* SNIPPET_TFIDF_H_ */
