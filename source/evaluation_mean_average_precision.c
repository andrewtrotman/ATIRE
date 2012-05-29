/*
	EVALUATION_MEAN_AVERAGE_PRECISION.C
	-----------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "evaluation_mean_average_precision.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_document.h"

/*
	ANT_EVALUATION_MEAN_AVERAGE_PRECISION::EVALUATE()
	-------------------------------------------------
*/
double ANT_evaluation_mean_average_precision::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
unsigned long long current;
long long found_and_relevant;
double precision;

if ((got = setup(topic)) == NULL)
	return 0;
if (got->number_of_relevant_documents[subtopic] == 0)
	return 0;

key.topic = topic;
key.subtopic = got->subtopics[subtopic];
precision = 0;
current = found_and_relevant = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next())
	{
	current++;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		{
		/*
			At this point we have an assessment for the document, but it might have been assessed are irrelevant
		*/
		if (relevance_data->relevant_characters != 0)
			{
			/*
				At this point we know it was relevant
			*/
			found_and_relevant++;
			precision += (double)found_and_relevant / (double)current;
			}
		}
	}
return precision / got->number_of_relevant_documents[subtopic];
}
