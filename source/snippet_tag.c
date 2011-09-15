/*
	SNIPPET_TAG.C
	-------------
*/
#include "ctypes.h"
#include "parser.h"
#include "snippet_tag.h"

/*
	ANT_SNIPPET_TAG::ANT_SNIPPET_TAG()
	----------------------------------
*/
ANT_snippet_tag::ANT_snippet_tag(unsigned long max_length, long length_of_longest_document, char *tag) : ANT_snippet(length_of_longest_document)
{
parser = new ANT_parser();
maximum_snippet_length = max_length;
this->tag = strnew(tag);
tag_length = strlen(tag);
}

/*
	ANT_SNIPPET_TAG::~ANT_SNIPPET_TAG()
	-----------------------------------
*/
ANT_snippet_tag::~ANT_snippet_tag()
{
delete parser;
delete [] tag;
}

/*
	ANT_SNIPPET_TAG::GET_SNIPPET()
	------------------------------
*/
char *ANT_snippet_tag::get_snippet(char *snippet, char *document, char *query)
{
char *into, *start;
ANT_parser_token *token;
size_t substring_length, length_in_bytes;
long found_title;

/*
	Initialise
*/
length_in_bytes = 0;
substring_length = 0;
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
		if (token->type == TT_TAG_OPEN && tag_length == token->length() && strnicmp(token->string(), tag, tag_length) == 0)
			found_title = true;
		}
	else
		{
		if (token->type == TT_TAG_OPEN || token->type == TT_TAG_CLOSE)
			{
			if (token->type == TT_TAG_CLOSE && tag_length == token->length() - 1 && strnicmp(token->string() + 1, tag, tag_length) == 0)  // we'e at the close tag
				break;
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
	Remove duplicaate whitespace and return
*/
return strip_duplicate_space_inline(snippet);
}
