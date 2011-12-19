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
ANT_snippet_beginning::ANT_snippet_beginning(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag) : ANT_snippet(max_length, length_of_longest_document, engine, stemmer)
{
this->tag = strnew(tag);
tag_length = strlen(tag);
}

/*
	ANT_SNIPPET_BEGINNING::~ANT_SNIPPET_BEGINNING()
	-----------------------------------------------
*/
ANT_snippet_beginning::~ANT_snippet_beginning()
{
delete [] tag;
}

/*
	ANT_SNIPPET_BEGINNING::GET_SNIPPET()
	------------------------------------
	Find the end of the first instance of the given element and then generate the snippet as the text from that point on.
	This allows you to say, for example, "the snppet starts after the <title> tag"
	
*/
char *ANT_snippet_beginning::get_snippet(char *snippet, char *document)
{
char *from;
ANT_parser_token *token;

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	copy all non-XML-tag content from the start of the document until we fill the snippet.
*/
while ((token = parser->get_next_token()) != NULL)
	{
	/*
		Find the end of the title tag
	*/
	if (token->type == TT_TAG_CLOSE && tag_length == token->length() - 1 && strnicmp(token->string() + 1, tag, tag_length) == 0)
		{
		from = strchr(token->string() + token->length(), '>');
		XML_to_text(document_text, from);
		parser->set_document(document_text);
		next_n_characters_after(snippet, maximum_snippet_length);
		break;
		}
	}

/*
	Remove duplicaate whitespace and return
*/
return strip_duplicate_space_inline(snippet);
}
