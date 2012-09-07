/*
	EVALUATION_DISCOUNTED_CUMULATIVE_GAIN.C
	---------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "evaluation_discounted_cumulative_gain.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_DISCOUNTED_CUMULATIVE_GAIN::EVALUATE()
	-----------------------------------------------------
*/
double ANT_evaluation_discounted_cumulative_gain::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
double discounted_cumulative_gain = 0;
long long current;

if ((got = setup(topic, subtopic)) == NULL)
	return 0;

key.topic = topic;
key.subtopic = subtopic;
current = 0;

for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			discounted_cumulative_gain += gain(relevance_data->relevant_characters, current) * discount(current);

return discounted_cumulative_gain;
}

/*
	ANT_EVALUATION_DISCOUNTED_CUMULATIVE_GAIN::GAIN()
	-------------------------------------------------
	Calculates the alpha-gain value for a given relevance in a given rank
*/
double ANT_evaluation_discounted_cumulative_gain::gain(long long relevance, unsigned long long rank)
{
double g;

if (trec_mode)
	g = (double)ANT_pow2_64(relevance) - 1;
else
	g = (double)relevance;

return g * pow(1 - alpha, (double)rank);
}

/*
	ANT_EVALUATION_DISCOUNTED_CUMULATIVE_GAIN::DISCOUNT()
	-----------------------------------------------------
*/
double ANT_evaluation_discounted_cumulative_gain::discount(unsigned long long rank)
{
if (trec_mode)
	return log(2.0) / log(rank + 2.0);
else
	return 1.0 / ANT_log2(2.0 + rank);
}

/*
	ANT_EVALUATION_DISCOUNTED_CUMULATIVE_GAIN::GAIN_COMPARE()
	---------------------------------------------------------
*/
int ANT_evaluation_discounted_cumulative_gain::gain_compare(const void *a, const void *b)
{
double one, two;

one = *(double *)a;
two = *(double *)b;

return (one < two) - (one > two);
}
