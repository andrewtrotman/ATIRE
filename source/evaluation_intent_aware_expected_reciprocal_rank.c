/*
	EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK.C
	--------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evaluation_expected_reciprocal_rank.h"
#include "evaluation_intent_aware_expected_reciprocal_rank.h"
#include "search_engine_result_iterator.h"
#include "relevant_document.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "pragma.h"

/*
	ANT_EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK::EVALUATE()
	----------------------------------------------------------------
*/
double ANT_evaluation_intent_aware_expected_reciprocal_rank::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got_topic, key_topic;
ANT_relevant_document key, *relevance_data;
ANT_relevant_subtopic *got;
long long current_subtopic, i;
double *subtopicGain, *idealIdeal, *err;
double idealIdealGain, score, precision;
const double alpha = 0.5;

key_topic.topic = topic;

got_topic = (ANT_relevant_topic *)bsearch(&key_topic, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);
if (got_topic == NULL)
	{
	printf("Didn't find topic %ld in relevant topic list!\n", topic);
	return 0;
	}

#if 1
/*
	This is the calculation that's done in TREC's provided ndeval.c
	I don't believe this is the correct calculation, but we do it for consistency.
*/
subtopicGain = new double[got_topic->number_of_subtopics];
idealIdeal = new double[precision_point];
idealIdealGain = (double)got_topic->number_of_subtopics;
err = new double[precision_point];

for (i = 0; i < precision_point; i++)
	err[i] = 0.0;

for (i = 0; i < got_topic->number_of_subtopics; i++)
	subtopicGain[i] = 1.0;

key.topic = topic;
for (i = 0, key.docid = iterator.first(search_engine); key.docid >= 0 && i < precision_point; key.docid = iterator.next(), i++)
	{
	score = 0;
	// for each subtopic
	for (current_subtopic = 0; current_subtopic < got_topic->number_of_subtopics; current_subtopic++)
		{
		key.subtopic = got_topic->subtopic_list[current_subtopic].subtopic;

		if ((got = setup(key.topic, key.subtopic)) == NULL)
			continue;

		if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
			if (relevance_data->relevant_characters > 0)
				{
				score += subtopicGain[current_subtopic];
				subtopicGain[current_subtopic] *= 1 - alpha;
				}
		}
	err[i] = score / (i + 1.0);
	}

for (i = 0; i < precision_point; i++)
	{
	idealIdeal[i] = idealIdealGain / (i + 1.0);
	idealIdealGain *= 1 - alpha;
	}

for (i = 1; i < precision_point; i++)
	{
	err[i] += err[i - 1];
	idealIdeal[i] += idealIdeal[i - 1];
	}

for (i = 1; i < precision_point; i++)
	err[i] /= idealIdeal[i];

precision = err[precision_point - 1];

delete [] subtopicGain;
delete [] idealIdeal;
delete [] err;

return precision;
#else
/*
	This is what I believe the ERR-IA calculation _should_ be
*/
for (current_subtopic = 0; current_subtopic < got_topic->number_of_subtopics; current_subtopic++)
	precision += ANT_evaluation_expected_reciprocal_rank::evaluate(search_engine, topic, (long)got_topic->subtopic_list[current_subtopic].subtopic);

return precision / got_topic->number_of_subtopics;
#endif

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
