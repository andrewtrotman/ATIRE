/*
	EVALUATION_EXPECTED_RECIPROCAL_RANK.C
	-------------------------------------
*/
#include <stdlib.h>
#include "evaluation_expected_reciprocal_rank.h"
#include "search_engine_result_iterator.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"
#include "maths.h"

/*
	ANT_EVALUATION_EXPECTED_RECIPROCAL_RANK::EVALUATE()
	---------------------------------------------------
*/
double ANT_evaluation_expected_reciprocal_rank::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
long long current;
double r, score = 0.0, decay = 1.0;

if ((got = setup(topic, subtopic)) == NULL)
	return 0;

key.topic = topic;
key.subtopic = subtopic;

current = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
		{
		r = (ANT_pow2((long)relevance_data->relevant_characters) - 1.0) / ANT_pow2((long)maximum_judgement);
		score += r * decay / (current + 1.0);
		decay *= (1 - r);
		}

return score;
}
