/*
	FOCUS_LOWEST_TAG.C
	------------------
*/
#include "focus_lowest_tag.h"
#include "parser.h"

/*
	ANT_FOCUS_LOWEST_TAG::FOCUS()
	-----------------------------
*/
ANT_focus_result *ANT_focus_lowest_tag::focus(unsigned char *document, ANT_focus_result *result)
{
ANT_string_pair *token;
long found_first, find_last;

find_last = found_first = FALSE;

result->start = result->finish = (char *)document;
parser.set_document(document);

while ((token = parser.get_next_token()) != NULL)
	{
	if (ANT_isupper(token->start[0]))
		{
		if (!found_first)
			{
			result->start = token->start;
			while (*result->start != '<' && result->start > (char *)document)
				result->start--;
			}
		}
	else if (token->start[0] == '/')		// end token
		{
		if (find_last)
			{
			find_last = FALSE;
			if ((result->finish = strchr(token->start + token->length(), '>')) != NULL)
				result->finish++;		// after the '>'
			else
				result->finish = token->start + strlen(token->start);		// on error use the end of the document
			}
		}
	else if (match(token))
		find_last = found_first = TRUE;
	}

/*
	If we didn't find anything then use end of document
*/
if (result->finish == (char *)document)
	result->finish = (char *)document + strlen((char *)document);
if (!found_first)
	result->start = (char *)document;

return result;
}

