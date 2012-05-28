/*
	EVALUATION_PRECISION_AT_N.C
	---------------------------
*/
#include <stdlib.h>
#include "evaluation_precision_at_n.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_document.h"

/*
	ANT_EVALUATION_PRECISION_AT_N::EVALUATE()
	-----------------------------------------
*/
double ANT_evaluation_precision_at_n::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_relevant, current;

if ((got = setup(topic)) == NULL)
	return 0;

key.topic = topic;
key.subtopic = got->subtopics[subtopic];

current = found_and_relevant = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			found_and_relevant++;

return (double)found_and_relevant / (double)precision_point;		// we're computing p@n so divide by n
}
