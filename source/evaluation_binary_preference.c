/*
	EVALUATION_BINARY_PREFERENCE.C
	------------------------------
*/
#include <stdlib.h>
#include "evaluation_binary_preference.h"
#include "search_engine_result_iterator.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_BINARY_PREFERENCE::EVALUATE()
	--------------------------------------------
*/
double ANT_evaluation_binary_preference::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_nonrelevant, total_nonrelevant, total_relevant, current;
double precision, denominator;

if ((got = setup(topic, subtopic)) == NULL)
	return 0;

if ((total_nonrelevant = got->number_of_nonrelevant_documents) == 0)
	return 1;	// topic has no non-relevant documents so they are all relevant so we score a perfect score

if ((total_relevant = got->number_of_relevant_documents) == 0)
	return 0; // topic has no relevant documents, so all are irrelevant so we score 0


key.topic = topic;
key.subtopic = subtopic;

precision = 0;
current = found_and_nonrelevant = 0;
denominator = (double)minimum + ANT_min(total_relevant, total_nonrelevant);

for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters == 0)
			found_and_nonrelevant++;
		else
			precision += 1.0 - ((minimum + (double)ANT_min(found_and_nonrelevant, total_relevant)) / denominator);

return precision / total_relevant;
}
