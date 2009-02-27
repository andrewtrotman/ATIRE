/*
	MEAN_AVERAGE_PRECISION.C
	------------------------
*/
#include <stdio.h>
#include <string.h>

#include "mean_average_precision.h"
#include "memory.h"
#include "relevant_topic.h"
#include "relevant_document.h"
#include "search_engine.h"
#include "search_engine_accumulator.h"

/*
	ANT_MEAN_AVERAGE_PRECISION::ANT_MEAN_AVERAGE_PRECISION()
	--------------------------------------------------------
*/
ANT_mean_average_precision::ANT_mean_average_precision(ANT_memory *memory, ANT_relevant_document *relevance_list, long relevance_list_length)
{
long current, last_topic, current_topic, relevant_documents;

/*
	Take a copy of the relevance_list and sort it on topic then docid.
*/
this->memory = memory;
this->relevance_list = (ANT_relevant_document *)memory->malloc(sizeof(*relevance_list) * relevance_list_length);
this->relevance_list_length = relevance_list_length;
memcpy(this->relevance_list, relevance_list, sizeof(*relevance_list) *  relevance_list_length);
qsort(this->relevance_list, relevance_list_length, sizeof(*this->relevance_list), ANT_relevant_document::compare);

/*
	Find out how many topics there are in the list
*/
last_topic = -1;
topics_list_length = 0;
for (current = 0; current < relevance_list_length; current++)
	if (this->relevance_list[current].topic != last_topic)
		{
		last_topic = this->relevance_list[current].topic;
		topics_list_length++;
		}

/*
	Allocarte memory for them and copy them in
*/
topics = (ANT_relevant_topic *)memory->malloc(sizeof(*topics) * topics_list_length);

last_topic = this->relevance_list[0].topic;	// init with the first topic
relevant_documents = 1;
current_topic = 0;
for (current = 1; current < relevance_list_length; current++)
	if (this->relevance_list[current].topic != last_topic)
		{
		topics[current_topic].topic = last_topic;
		topics[current_topic].number_of_relevant_documents = relevant_documents;
		current_topic++;
		last_topic = this->relevance_list[current].topic;
		relevant_documents = 1;
		}
	else
		relevant_documents++;

topics[current_topic].topic = last_topic;
topics[current_topic].number_of_relevant_documents = relevant_documents;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::AVERAGE_PRECISION()
	-----------------------------------------------
*/
double ANT_mean_average_precision::average_precision(long topic, ANT_search_engine *search_engine)
{
ANT_search_engine_accumulator *accumulators, **results_list;
ANT_relevant_document key;
ANT_relevant_topic topic_key, *got;
long current, found_and_relevant, results_list_length;
double precision;

key.topic = topic;
precision = 0;
found_and_relevant = 0;
results_list = search_engine->accumulator_pointers;
accumulators = search_engine->accumulator;
results_list_length = search_engine->document_count();

for (current = 0; current < results_list_length; current++)
	if (!results_list[current]->is_zero_rsv())
		{
		key.docid = results_list[current] - accumulators;
		if (bsearch(&key, relevance_list, relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare) != NULL)
			{
			found_and_relevant++;
			precision += (double)found_and_relevant / (double)(current + 1);
			}
		}

topic_key.topic = topic;
got = (ANT_relevant_topic *)bsearch(&topic_key, topics, topics_list_length, sizeof(topic_key), ANT_relevant_topic::compare);
if (got == NULL)
	{
	puts("Unexpected: Topic not found in topic list");
	precision = 0;
	}
else
	precision /= got->number_of_relevant_documents;

return precision;
}


/*
	ANT_MEAN_AVERAGE_PRECISION::AVERAGE_GENERALISED_PRECISION()
	-----------------------------------------------------------
*/
double ANT_mean_average_precision::average_generalised_precision(long topic, ANT_search_engine *search_engine)
{
ANT_search_engine_accumulator *accumulators, **results_list;
ANT_relevant_document key, *relevance_data;
ANT_relevant_topic topic_key, *got;
long current, results_list_length;
double precision, doc_precision, doc_recall, doc_f_score, found_and_relevant;
const double beta = 0.25;

key.topic = topic;
precision = 0;
found_and_relevant = 0;
results_list = search_engine->accumulator_pointers;
accumulators = search_engine->accumulator;
results_list_length = search_engine->document_count();

for (current = 0; current < results_list_length; current++)
	if (!results_list[current]->is_zero_rsv())
		{
		key.docid = results_list[current] - accumulators;
		if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
			{
			doc_precision = (double)relevance_data->relevant_characters / (double)relevance_data->document_length;
			doc_recall = 1.0;		// we retrieve the whole document so recall is 1.
			doc_f_score = (1.0 + beta * beta ) * (doc_precision  * doc_recall) / (beta * beta * doc_precision + doc_recall);
			found_and_relevant += doc_f_score;
			precision += (double)found_and_relevant / (double)(current + 1);
			}
		}

topic_key.topic = topic;
got = (ANT_relevant_topic *)bsearch(&topic_key, topics, topics_list_length, sizeof(topic_key), ANT_relevant_topic::compare);
if (got == NULL)
	{
	puts("Unexpected: Topic not found in topic list");
	precision = 0;
	}
else
	precision /= got->number_of_relevant_documents;

return precision;
}

