/*
	EVALUATION.C
	------------
*/
#include <new>
#include "evaluation.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "search_engine.h"
#include "pragma.h"

/*
	ANT_EVALUATION::SET_LISTS()
	---------------------------
*/
void ANT_evaluation::set_lists(ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length)
{
this->relevant_topic_list = relevant_topic_list;
this->relevant_topic_list_length = relevant_topic_list_length;
}

/*
	ANT_EVALUATION::SETUP()
	-----------------------
*/
ANT_relevant_subtopic *ANT_evaluation::setup(long long topic, long long subtopic)
{
ANT_relevant_topic topic_key, *got_topic;
ANT_relevant_subtopic subtopic_key, *got_subtopic;

topic_key.topic = topic;
got_topic = (ANT_relevant_topic *)bsearch(&topic_key, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(topic_key), ANT_relevant_topic::compare);
if (got_topic == NULL)
	{
	fprintf(stderr, "Unexpected: Topic '%lld' not found in qrels - No relevant docs for query?\n", topic);
	return NULL;
	}

subtopic_key.topic = topic;
subtopic_key.subtopic = subtopic;
got_subtopic = (ANT_relevant_subtopic *)bsearch(&subtopic_key, got_topic->subtopic_list, (size_t)got_topic->number_of_subtopics, sizeof(subtopic_key), ANT_relevant_subtopic::compare);

return got_subtopic;
}

/*
	ANT_EVALUATION::EVALUATE()
	--------------------------
*/
double ANT_evaluation::evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic)
{
*valid = false;
return 0;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
