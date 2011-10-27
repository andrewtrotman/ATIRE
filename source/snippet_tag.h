/*
	SNIPPET_TAG.H
	-------------
*/
#ifndef SNIPPET_TAG_H_
#define SNIPPET_TAG_H_

#include "snippet.h"

class ANT_parser;

/*
	class ANT_SNIPPET_TAG
	---------------------
*/
class ANT_snippet_tag : public ANT_snippet
{
private:
	char *tag;									// only use content in the first occurence of this tag
	size_t tag_length;							// this is the length of this->tag

public:
	ANT_snippet_tag(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag);
	virtual ~ANT_snippet_tag();

	virtual char *get_snippet(char *snippet, char *document);
	virtual ANT_NEXI_term_ant **parse_query(char *query) { (void)query; return NULL; }
} ;

#endif /* SNIPPET_TAG_H_ */
