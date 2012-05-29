/*
	EVALUATION.C
	------------
*/
#include <new>
#include "evaluation.h"
#include "relevant_topic.h"
#include "relevant_document.h"
#include "search_engine.h"
#include "search_engine_result_iterator.h"
#include "memory.h"
#include "maths.h"
#include "pragma.h"

/*
	ANT_EVALUATION::SET_LISTS()
	---------------------------
*/
void ANT_evaluation::set_lists(ANT_relevant_document *relevance_list, long long relevance_list_length, ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length)
{
this->relevance_list = relevance_list;
this->relevance_list_length = relevance_list_length;
this->relevant_topic_list = relevant_topic_list;
this->relevant_topic_list_length = relevant_topic_list_length;
}

/*
	ANT_EVALUATION::SETUP()
	-----------------------
*/
ANT_relevant_topic *ANT_evaluation::setup(long long topic)
{
ANT_relevant_topic topic_key, *got;

topic_key.topic = topic;

got = (ANT_relevant_topic *)bsearch(&topic_key, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(topic_key), ANT_relevant_topic::compare);
if (got == NULL)
	fprintf(stderr, "Unexpected: Topic '%ld' not found in qrels - No relevant docs for query?\n", topic);

return got;
}

/*
	ANT_EVALUATION::EVALUATE()
	--------------------------
*/
double ANT_evaluation::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
return 0;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
