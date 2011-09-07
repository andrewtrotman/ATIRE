/*
	SNIPPET_BEGINNING.C
	-------------------
*/
#include "ctypes.h"
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
char *into, *start;
ANT_parser_token *token;
unsigned long length_in_bytes, substring_length;
long found_title;

/*
	Initialise
*/
length_in_bytes = substring_length = 0;
into = snippet;
found_title = false;
start = NULL;

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	copy all non-XML-tag content from the stat of the document until we fill the snippet.
*/
while ((token = parser->get_next_token()) != NULL)
	{
	/*
		First find the end of the title tag
	*/
	if (!found_title)
		{
		if (token->type == TT_TAG_CLOSE && strnicmp(token->string() + 1, "DOCNO", 4) == 0)
			found_title = true;
		}
	else
		{
		if (token->type == TT_TAG_OPEN || token->type == TT_TAG_CLOSE)
			{
			/*
				Cut out XML tags by copying the remaining content
			*/
			if (start != NULL)
				{
				strncpy(into, start, substring_length);
				into += substring_length;
				*into++ = ' ';
				length_in_bytes += substring_length + 1;			// +1 to include the space
				}
			substring_length = 0;
			start = NULL;
			}
		else if (token->type == TT_WORD || token->type == TT_NUMBER)
			{
			/*
				Include text and numbers
			*/
			if (start == NULL)
				start = token->string();
	
			if (length_in_bytes + (token->string() + token->length() - start) >= maximum_snippet_length)
				break;

			substring_length = token->string() + token->length() - start;
			}
		}
	}

/*
	Tack the final content on the end
*/
if (start != NULL)
	{
	strncpy(into, start, substring_length);
	into += substring_length;
	}

*into = '\0';

/*
	Remove CR/LF as the just screw up the look of the snippet
*/
start = into = snippet;
while (*start != '\0')
	{
	*into = *start++;
	if (ANT_isspace(*into))
		*into = ' ';				// replace all white space with ' ' (remove CF/LF)
	if (!(ANT_isspace(*into) && ANT_isspace(*start)))		// skip over multiple spaces
		into++;
	}
*into = '\0';

/*
	return the snippet
*/
return snippet;
}
