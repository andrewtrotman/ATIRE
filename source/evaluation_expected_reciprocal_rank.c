/*
	EVALUATION_EXPECTED_RECIPROCAL_RANK.C
	-------------------------------------
*/
#include <stdlib.h>
#include "evaluation_expected_reciprocal_rank.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_EXPECTED_RECIPROCAL_RANK::EVALUATE()
	---------------------------------------------------
*/
double ANT_evaluation_expected_reciprocal_rank::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
double r, score = 0.0, decay = 1.0;
long current;

if ((got = setup(topic)) == NULL)
	return 0;

key.topic = topic;
key.subtopic = got->subtopics[subtopic];
current = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		{
		r = (ANT_pow2((long)relevance_data->relevant_characters) - 1.0) / ANT_pow2(maximum_judgement);
		score += r * decay / (current + 1.0);
		decay *= (1 - r);
		}

return score;
}
