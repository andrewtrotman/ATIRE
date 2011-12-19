/*
	FOCUS_LOWEST_TAG.C
	------------------
*/
#include "focus_results_list.h"
#include "focus_lowest_tag.h"
#include "parser.h"

/*
	ANT_FOCUS_LOWEST_TAG::FOCUS()
	-----------------------------
*/
ANT_focus_result *ANT_focus_lowest_tag::focus(unsigned char *document, long *results_length, long long docid, char *document_name, ANT_search_engine_accumulator *document_accumulator)
{
ANT_parser_token *token;
ANT_focus_result *result;
long found_first, find_last;

/*
	get a results object
*/
if ((result = result_factory->new_result()) == NULL)
	{
	*results_length = 0;
	return NULL;
	}

/*
	There can be only one passage in this case
*/
*results_length = 1;
find_last = found_first = FALSE;

/*
	By default the passage starts at the beginning of the document
*/
result->start = result->finish = (char *)document;
result->set_rsv(document_accumulator == NULL ? 1 : document_accumulator->get_rsv());
result->docid = docid;
result->document_name = document_name;

/*
	now go looking in the document
*/
parser.set_document(document);

while ((token = parser.get_next_token()) != NULL)
	{
	if (token->type == TT_TAG_OPEN)
		{
		if (!found_first)
			{
#ifndef NEVER
			/*
				This is before-the-tag (only used for display purposes)
			*/
			result->start = token->start;
			while (*result->start != '<' && result->start > (char *)document)
				result->start--;
#else
			/*
				This is the first-character-of-tag (used for INEX Focused Retrieval)
			*/
			result->start = strchr(token->start, '>');
#endif
			}
		}
	else if (token->type == TT_TAG_CLOSE)
		{
		if (find_last)
			{
			find_last = FALSE;
			if ((result->finish = strchr(token->start + token->length(), '>')) != NULL)
				result->finish++;											// after the '>'
			else
				result->finish = token->start + strlen(token->start);		// on error use the end of the document
			}
		}
	else if	((token->type == TT_WORD || token->type == TT_NUMBER) && match(token->normalized_pair()))
		find_last = found_first = TRUE;
	}

/*
	If we didn't find anything then the passage finished at the end of document
*/
if (result->finish == (char *)document)
	result->finish = (char *)document + strlen((char *)document);
if (!found_first)
	result->start = (char *)document;

return result;
}

