/*
	SNIPPET.H
	---------
*/
#ifndef SNIPPET_H_
#define SNIPPET_H_

#include "NEXI_ant.h"
class ANT_NEXI_term_ant;

/*
	class ANT_SNIPPET
	-----------------
*/
class ANT_snippet
{
protected:
	ANT_NEXI_ant NEXI_parser;

protected:
	ANT_NEXI_term_ant **generate_term_list(char *query, long *terms_in_query);
	static int cmp_term(const void *a, const void *b);		// a is a (ANT_parser_token *) and b is a (ANT_NEXI_term_ant **)

public:
	ANT_snippet() {}
	virtual ~ANT_snippet() {}

	virtual char *get_snippet(char *snippet, char *document, char *query) = 0;
} ;


#endif /* SNIPPET_H_ */
