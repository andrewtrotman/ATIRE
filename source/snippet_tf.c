/*
	SNIPPET_TF.C
	------------
*/
#include "ctypes.h"
#include "parser.h"
#include "snippet_tf.h"

/*
	ANT_SNIPPET_TF::ANT_SNIPPET_TF()
	--------------------------------
*/
ANT_snippet_tf::ANT_snippet_tf(unsigned long max_length, long length_of_longest_document) : ANT_snippet(length_of_longest_document)
{
parser = new ANT_parser();
maximum_snippet_length = max_length;
this->length_of_longest_document = length_of_longest_document;
}

/*
	ANT_SNIPPET_TF::~ANT_SNIPPET_TF()
	---------------------------------
*/
ANT_snippet_tf::~ANT_snippet_tf()
{
delete parser;
}

/*
	ANT_SNIPPET_TF::GET_SNIPPET()
	-----------------------------
*/
char *ANT_snippet_tf::get_snippet(char *snippet, char *document, char *query)
{
long query_length, found;
ANT_NEXI_term_ant **term_list;
ANT_parser_token *token;

/*
	get a list of all the search terms out of the query
*/
term_list = generate_term_list(query, &query_length);

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	Remove all XML tags
*/
found = 0;
while ((token = parser->get_next_token()) != NULL)
	if (token->type == TT_WORD || token->type == TT_NUMBER)
		if (bsearch(token, term_list, query_length, sizeof(*term_list), cmp_term) != NULL)
			keyword_hit[found++] = token->string();

*snippet = '\0';

return snippet;
}
