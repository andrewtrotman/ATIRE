/*
	EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN.C
	--------------------------------------------------
*/
#include <stdlib.h>
#include "evaluation_discounted_cumulative_gain.h"
#include "evaluation_normalised_discounted_cumulative_gain.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::EVALUATE()
	----------------------------------------------------------------
*/
double ANT_evaluation_normalised_discounted_cumulative_gain::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_relevant_topic *topic_got, topic_key;

topic_key.topic = topic;

topic_got = (ANT_relevant_topic *)bsearch(&topic_key, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);

return ANT_evaluation_discounted_cumulative_gain::evaluate(search_engine, topic, subtopic) / ideal_gains[topic_got - relevant_topic_list];
}

/*
	ANT_EVALUATION_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::SET_LISTS()
	-----------------------------------------------------------------
	Calculate the ideal gain for each topic
*/
void ANT_evaluation_normalised_discounted_cumulative_gain::set_lists(ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length)
{
long long topic, document, offset;
ANT_relevant_subtopic *subtopic;
double *this_topic_ideal_gain = NULL;

ANT_evaluation::set_lists(relevant_topic_list, relevant_topic_list_length);

ideal_gains = new double[relevant_topic_list_length];
for (topic = 0; topic < relevant_topic_list_length; topic++)
	{
	subtopic = relevant_topic_list[topic].subtopic_list;
	ideal_gains[topic] = 0;

	delete [] this_topic_ideal_gain;
	this_topic_ideal_gain = new double[subtopic->number_of_relevant_documents];

	for (offset = 0, document = 0; document < subtopic->number_of_documents; document++)
		if (subtopic->document_list[document].relevant_characters != 0)
			this_topic_ideal_gain[offset++] = (double)subtopic->document_list[document].relevant_characters;

	qsort(this_topic_ideal_gain, offset, sizeof(*this_topic_ideal_gain), ANT_evaluation_discounted_cumulative_gain::gain_compare);

	for (offset = 0; offset < subtopic->number_of_relevant_documents && offset < precision_point; offset++)
		ideal_gains[topic] += gain((long long)this_topic_ideal_gain[offset], offset) * discount(offset);
	}

delete [] this_topic_ideal_gain;
}
