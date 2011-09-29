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
ANT_snippet_tag::ANT_snippet_tag(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag) : ANT_snippet(max_length, length_of_longest_document, engine, stemmer)
{
this->tag = strnew(tag);
tag_length = strlen(tag);
}

/*
	ANT_SNIPPET_TAG::~ANT_SNIPPET_TAG()
	-----------------------------------
*/
ANT_snippet_tag::~ANT_snippet_tag()
{
delete [] tag;
}

/*
	ANT_SNIPPET_TAG::GET_SNIPPET()
	------------------------------
	The snippet starts at the contents of the first occurrence of the given tag
*/
char *ANT_snippet_tag::get_snippet(char *snippet, char *document)
{
char *start, *copy;
ANT_parser_token *token, sentinal;
long found_title;
size_t content_length;

/*
	Initialise
*/
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
			{
			start = strchr(token->string() + token->length(), '>');
			found_title = true;
			}
		}
	else
		if (token->type == TT_TAG_CLOSE && tag_length == token->length() - 1 && strnicmp(token->string() + 1, tag, tag_length) == 0)
			break;
	}

/*
	If the given tag didn't exist in the document the use the start of the document
*/
if (start == NULL)
	start = document;
if (token == NULL)
	{
	token = &sentinal;
	sentinal.start = document + strlen(document);
	}

/*
	Cut the element out of the document and convert it into text
*/
content_length = token->string() - start;
copy = new char [content_length + 1];
strncpy(copy, start, content_length);
copy[content_length] = '\0';
XML_to_text(document_text, copy);
delete [] copy;

/*
	Now generate the snippet
*/
parser->set_document(document_text);
next_n_characters_after(snippet, maximum_snippet_length);

/*
	Remove duplicaate whitespace and return
*/
return strip_duplicate_space_inline(snippet);
}
