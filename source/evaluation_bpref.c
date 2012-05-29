/*
	EVALUATION_BPREF.C
	------------------
*/
#include <stdlib.h>
#include "evaluation_bpref.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_BPREF::EVALUATE()
	--------------------------------
*/
double ANT_evaluation_bpref::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_nonrelevant, total_nonrelevant, total_relevant;
unsigned long long current;
double precision, denominator;

if ((got = setup(topic)) == NULL)
	return 0;

if ((total_nonrelevant = got->number_of_nonrelevant_documents[subtopic]) == 0)
	return 1;	// topic has no non-relevant documents so they are all relevant so we score a perfect score

if ((total_relevant = got->number_of_relevant_documents[subtopic]) == 0)
	return 0; // topic has no relevant documents, so all are irrelevant so we score 0

key.topic = topic;
key.subtopic = got->subtopics[subtopic];

precision = 0;
current = found_and_nonrelevant = 0;
denominator = (double)minimum + ANT_min(total_relevant, total_nonrelevant);

for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters == 0)
			found_and_nonrelevant++;
		else
			precision += 1.0 - ((minimum + (double)ANT_min(found_and_nonrelevant, total_relevant)) / denominator);

return precision / total_relevant;
}
