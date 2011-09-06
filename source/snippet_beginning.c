/*
	SNIPPET_BEGINNING.C
	-------------------
*/
#include "parser.h"
#include "snippet_beginning.h"

/*
	ANT_SNIPPET_BEGINNING::ANT_SNIPPET_BEGINNING()
	----------------------------------------------
*/
ANT_snippet_beginning::ANT_snippet_beginning(unsigned long max_length)
{
parser = new ANT_parser();
maximum_snippet_length = max_length;
}

/*
	ANT_SNIPPET_BEGINNING::~ANT_SNIPPET_BEGINNING()
	-----------------------------------------------
*/
ANT_snippet_beginning::~ANT_snippet_beginning()
{
delete parser;
}

/*
	ANT_SNIPPET_BEGINNING::GET_SNIPPET()
	------------------------------------
*/
char *ANT_snippet_beginning::get_snippet(char *snippet, char *document)
{
char *into;
ANT_parser_token *token;
unsigned long length_in_bytes;
long found_title;

/*
	initialise
*/
length_in_bytes = 0;
into = snippet;
found_title = false;

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	Now for every token that a word or number, add it to the snippet
	this drops punctuation and tags
*/
while ((token = parser->get_next_token()) != NULL)
	if (token->type == TT_TAG_CLOSE)
		{
		if (strncmp(token->string(), "title", 5) == 0)
			found_title = true;
		}
	else if (found_title)
		if (token->type == TT_WORD || token->type == TT_NUMBER)
			{
			/*
				make sure it fits (including the '\0') then copy the token
			*/
			if (length_in_bytes + token->length() >= maximum_snippet_length)
				break;
			memcpy(into, token->string(), token->length());
			into += token->length();

			/*
				add a space on the end (which gets replaced with a '\0' on termination)
			*/
			*into++ = ' ';

			/*
				update the length of the string
			*/
			length_in_bytes += token->length() + 1;
			}

/*
	NULL terminate the string by replacing the final ' ' with '\0' (if there was one)
*/
if (length_in_bytes != 0)
	length_in_bytes--;
snippet[length_in_bytes] = '\0';

/*
	return the snippet
*/

return snippet;
}
