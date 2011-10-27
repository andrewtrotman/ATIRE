/*
	SNIPPET_BEGINNING.H
	-------------------
*/
#ifndef SNIPPET_BEGINNING_H_
#define SNIPPET_BEGINNING_H_


#include "snippet.h"

class ANT_parser;

/*
	class ANT_SNIPPET_BEGINNING
	---------------------------
*/
class ANT_snippet_beginning : public ANT_snippet
{
private:
	char *tag;									// ignore all text up-to the end of this tag
	size_t tag_length;							// this is the length of this->tag

public:
	ANT_snippet_beginning(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag);
	virtual ~ANT_snippet_beginning();

	virtual char *get_snippet(char *snippet, char *document);
	virtual ANT_NEXI_term_ant **parse_query(char *query) { (void)query; return NULL; }
} ;

#endif /* SNIPPET_BEGINNING_H_ */
