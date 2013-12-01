/*
	EVALUATION_INTENT_AWARE_PRECISION_AT_N.C
	----------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "evaluation_precision_at_n.h"
#include "evaluation_intent_aware_precision_at_n.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "pragma.h"

/*
	ANT_EVALUATION_INTENT_AWARE_PRECISION_AT_N::EVALUATE()
	------------------------------------------------------
*/
double ANT_evaluation_intent_aware_precision_at_n::evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic)
{
double precision = 0;
long long current_subtopic;
ANT_relevant_topic *got_topic, key_topic;

key_topic.topic = topic;

got_topic = (ANT_relevant_topic *)bsearch(&key_topic, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);
if (got_topic == NULL)
	{
	*valid = false;
	return 0;
	}

*valid = true;

for (current_subtopic = 0; current_subtopic < got_topic->number_of_subtopics; current_subtopic++)
	precision += ANT_evaluation_precision_at_n::evaluate(search_engine, topic, valid, (long)got_topic->subtopic_list[current_subtopic].subtopic);

return precision / got_topic->number_of_subtopics;

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
