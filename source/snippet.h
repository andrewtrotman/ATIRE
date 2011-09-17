/*
	SNIPPET.H
	---------
*/
#ifndef SNIPPET_H_
#define SNIPPET_H_

#include "NEXI_ant.h"

class ANT_NEXI_term_ant;
class ANT_parser;

/*
	class ANT_SNIPPET
	-----------------
*/
class ANT_snippet
{
protected:
	unsigned long maximum_snippet_length;		// length of the snippet in bytes
	ANT_parser *parser;							// parser used to parse documents
	ANT_NEXI_ant NEXI_parser;					// parser used to parse queries
	char **keyword_hit;							// list long enough to hold a pointer to each keyword in the document

protected:
	ANT_NEXI_term_ant **generate_term_list(char *query, long *terms_in_query);
	char *next_n_characters_after(char *snippet, long maximum_snippet_length);
	static int cmp_term(const void *a, const void *b);		// a is a (ANT_parser_token *) and b is a (ANT_NEXI_term_ant **)

public:
	ANT_snippet(unsigned long max_snippet_length, long length_of_longest_document);
	virtual ~ANT_snippet();

	virtual char *get_snippet(char *snippet, char *document, char *query) = 0;
} ;

#endif /* SNIPPET_H_ */
