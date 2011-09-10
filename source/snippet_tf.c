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
ANT_snippet_tf::ANT_snippet_tf(unsigned long max_length, long length_of_longest_document)
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
ANT_parser_token *token;

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	remove all XML tags
*/
while ((token = parser->get_next_token()) != NULL)
	if (token->type == TT_WORD || token->type == TT_NUMBER)
		{
		}

*snippet = '\0';

return snippet;
}
