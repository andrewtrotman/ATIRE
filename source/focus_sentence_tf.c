/*
	FOCUS_SENTENCE_TF.C
	-------------------
*/
#include "focus_results_list.h"
#include "focus_sentence_tf.h"
#include "parser.h"

/*
	ANT_FOCUS_SENTENCE_TF::FOCUS()
	------------------------------
*/
ANT_focus_result *ANT_focus_sentence_tf::focus(unsigned char *document, long *results_length, long long docid, char *document_name, ANT_search_engine_accumulator *document_accumulator)
{
ANT_focus_result *result;
ANT_string_pair *token;
long found_first, find_last;

if ((result = result_factory->new_result()) == NULL)
	{
	*results_length = 0;
	return NULL;
	}

*results_length = 1;					// number of passages we found in the document
find_last = found_first = FALSE;

result->start = result->finish = (char *)document;
result->set_rsv(document_accumulator == NULL ? 1 : document_accumulator->get_rsv());
result->docid = docid;
result->document_name = document_name;
parser.set_document(document);

while ((token = parser.get_next_token()) != NULL)
	{
	if (ANT_ispunct(token->start[0]))
		{
		}

	if (ANT_isupper(token->start[0]))
		{
		if (!found_first)
			{
#ifndef NEVER
			/*
				This is before-the-tag (used for display purposes)
			*/
			result->start = token->start;
			while (*result->start != '<' && result->start > (char *)document)
				result->start--;
#else
			/*
				This is the first-character-of-tag (used for INEX Focused Retrieval purposes)
			*/
			result->start = strchr(token->start, '>');
#endif
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
	else if (!ANT_ispunct(token->start[0]) && match(token))
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

