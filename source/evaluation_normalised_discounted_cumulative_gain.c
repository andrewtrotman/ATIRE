/*
	EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN.C
	--------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "evaluation_normalised_discounted_cumulative_gain.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::EVALUATE()
	----------------------------------------------------------------
*/
double ANT_evaluation_normalised_discounted_cumulative_gain::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
double discounted_cumulative_gain = 0;
long long current;

if ((got = setup(topic)) == NULL)
	return 0;

key.topic = topic;
key.subtopic = got->subtopics[subtopic];
current = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			if (trec_mode)
				discounted_cumulative_gain += (ANT_pow2(relevance_data->relevant_characters) - 1) / ANT_log2(2.0 + current);
			else
				discounted_cumulative_gain += relevance_data->relevant_characters / ANT_log2(2.0 + current);

return discounted_cumulative_gain / ideal_gains[got - relevant_topic_list][subtopic];
}

/*
	ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::SET_LISTS()
	-----------------------------------------------------------------
	In addition to what we normally do, we can calculate the ideal gain for each topic
*/
void ANT_evaluation_normalised_discounted_cumulative_gain::set_lists(ANT_relevant_document *relevance_list, long long relevance_list_length, ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length)
{
long current, topic, offset, subtopic;
ANT_relevant_topic *got;
double *this_topic_ideal = NULL;

ANT_evaluation::set_lists(relevance_list, relevance_list_length, relevant_topic_list, relevant_topic_list_length);

ideal_gains = new double *[relevant_topic_list_length];

for (topic = 0; topic < relevant_topic_list_length; topic++)
	{
	if ((got = setup(relevant_topic_list[topic].topic)) == NULL)
		continue;
	
	ideal_gains[topic] = new double[got->number_of_subtopics];
	for (subtopic = 0; subtopic < got->number_of_subtopics; subtopic++)
		{
		if (got->number_of_relevant_documents[subtopic] == 0)
			{
			ideal_gains[topic][subtopic] = 0;
			continue;
			}
		
		delete [] this_topic_ideal;
		this_topic_ideal = new double[got->number_of_relevant_documents[subtopic]];
		
		ideal_gains[topic][subtopic] = 0;
		
		for (offset = current = 0; current < got->number_of_relevant_documents[subtopic]; current++)
			{
			while (relevance_list[got->beginning_of_judgements[subtopic] + current + offset].relevant_characters == 0)
				offset++;
			this_topic_ideal[current] = relevance_list[got->beginning_of_judgements[subtopic] + current + offset].relevant_characters;
			}

		qsort(this_topic_ideal, got->number_of_relevant_documents[subtopic], sizeof(*this_topic_ideal), ANT_evaluation_normalised_discounted_cumulative_gain::gain_compare);
		
		for (current = 0; current < got->number_of_relevant_documents[subtopic] && current < precision_point; current++)
			if (trec_mode)
				ideal_gains[topic][subtopic] += (ANT_pow2(this_topic_ideal[current]) - 1) / ANT_log2(2.0 + current);
			else
				ideal_gains[topic][subtopic] += this_topic_ideal[current] / ANT_log2(2.0 + current);
		}
	}
}

/*
	ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::GAIN_COMPARE()
	--------------------------------------------------------------------
*/
int ANT_evaluation_normalised_discounted_cumulative_gain::gain_compare(const void *a, const void *b)
{
double one, two;

one = *(double *)a;
two = *(double *)b;

return (one < two) - (one > two);
}
