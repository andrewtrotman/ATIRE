/*
	EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK.C
	--------------------------------------------------
*/
#include <stdlib.h>
#include <string.h>
#include "evaluation_expected_reciprocal_rank.h"
#include "evaluation_intent_aware_expected_reciprocal_rank.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "pragma.h"

/*
	ANT_EVALUATION_INTENT_AWARE_EXPECTED_RECIPROCAL_RANK::EVALUATE()
	----------------------------------------------------------------
*/
double ANT_evaluation_intent_aware_expected_reciprocal_rank::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
double precision = 0;
long long current_subtopic;
ANT_relevant_topic *got_topic, key_topic;

key_topic.topic = topic;

got_topic = (ANT_relevant_topic *)bsearch(&key_topic, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);

for (current_subtopic = 0; current_subtopic < got_topic->number_of_subtopics; current_subtopic++)
	precision += ANT_evaluation_expected_reciprocal_rank::evaluate(search_engine, topic, got_topic->subtopic_list[current_subtopic].subtopic);

return precision / got_topic->number_of_subtopics;

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
