/*
	FOCUS_ARTICLE.C
	---------------
*/

#include "focus_results_list.h"
#include "focus_article.h"
#include "parser.h"

/*
	ANT_FOCUS_ARTICLE::FOCUS()
	--------------------------
	The focused result is the whole document,  This is the identify function
*/
ANT_focus_result *ANT_focus_article::focus(unsigned char *document, long *results_length, long long docid, char *document_name, ANT_search_engine_accumulator *document_accumulator)
{
ANT_focus_result *result;

/*
	Get a focused result object
*/
if ((result = result_factory->new_result()) == NULL)
	{
	*results_length = 0;
	return NULL;
	}

/*
	There is only one passage-result per document
*/
*results_length = 1;

/*
	That passage starts at the document start, finishes at the document, and had an rsv of the document's rsv
*/
result->start = (char *)document;
result->finish = (char *)document + strlen((char *)document);
result->set_rsv(document_accumulator == NULL ? 1 : document_accumulator->get_rsv());
result->docid = docid;
result->document_name = document_name;

return result;
}
