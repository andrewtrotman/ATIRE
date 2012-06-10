/*
	EVALUATION_MEAN_AVERAGE_PRECISION.C
	-----------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "evaluation_mean_average_precision.h"
#include "search_engine_result_iterator.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"

/*
	ANT_EVALUATION_MEAN_AVERAGE_PRECISION::EVALUATE()
	-------------------------------------------------
	Evaluate the results from search_engine for given topic and subtopic
*/
double ANT_evaluation_mean_average_precision::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_relevant, current;
double precision;

if ((got = setup(topic, subtopic)) == NULL)
	return 0;
if (got->number_of_relevant_documents == 0)
	return 0;

key.topic = topic;
key.subtopic = subtopic;

precision = 0;
current = found_and_relevant = 0;

for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next())
	{
	current++;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
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
return precision / got->number_of_relevant_documents;
}
