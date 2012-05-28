/*
	EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION.C
	------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "evaluation_mean_average_precision.h"
#include "evaluation_intent_aware_mean_average_precision.h"
#include "relevant_topic.h"

/*
	ANT_EVALUATION_INTENT_AWARE_MEAN_AVERAGE_PRECISION::EVALUATE()
	--------------------------------------------------------------
*/
double ANT_evaluation_intent_aware_mean_average_precision::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
long current_subtopic;
double precision = 0;
ANT_relevant_topic *got;

if ((got = setup(topic)) == NULL)
	return 0;

for (current_subtopic = 0; current_subtopic < got->number_of_subtopics; current_subtopic++)
	precision += ANT_evaluation_mean_average_precision::evaluate(search_engine, topic, current_subtopic);

return precision / got->number_of_subtopics;
}
