/*
	EVALUATION_SUCCESS_AT_N.C
	-------------------------
*/
#include <stdlib.h>
#include "evaluation_success_at_n.h"
#include "search_engine_result_iterator.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"

/*
	ANT_EVALUATION_SUCCESS_AT_N::EVALUATE()
	---------------------------------------
	This is an Otago "special" metric that returns 1 if the topic has at least one relevant
	document in the top n and 0 if it has none.  If all relevance is equal then it returns
	the success rate at point n.
*/
double ANT_evaluation_success_at_n::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_relevant, current;

if ((got = setup(topic, subtopic)) == NULL)
	return 0;

key.topic = topic;
key.subtopic = subtopic;
current = 0;
found_and_relevant = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			found_and_relevant++;

return found_and_relevant == 0 ? 0.0 : 1.0;
}
