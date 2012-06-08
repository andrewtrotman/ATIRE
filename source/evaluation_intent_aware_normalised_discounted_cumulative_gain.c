/*
	EVALUATION_INTENT_AWARE_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN.C
	---------------------------------------------------------------
*/
#include <stdlib.h>
#include <string.h>
#include "evaluation_discounted_cumulative_gain.h"
#include "evaluation_intent_aware_normalised_discounted_cumulative_gain.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"
#include "pragma.h"

/*
	ANT_EVALUATION_INTENT_AWARE_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::EVALUATE()
	-----------------------------------------------------------------------------
*/
double ANT_evaluation_intent_aware_normalised_discounted_cumulative_gain::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
double precision = 0;
long long current_subtopic;
ANT_relevant_topic *got_topic, key_topic;

key_topic.topic = topic;

got_topic = (ANT_relevant_topic *)bsearch(&key_topic, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);

for (current_subtopic = 0; current_subtopic < got_topic->number_of_subtopics; current_subtopic++)
	precision += ANT_evaluation_discounted_cumulative_gain::evaluate(search_engine, topic, got_topic->subtopic_list[current_subtopic].subtopic);

return precision / ideal_gains[got_topic - relevant_topic_list];

#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_EVALUATION_INTENT_AWARE_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::SET_LISTS()
	------------------------------------------------------------------------------
	Calculates a greedy approximation for the best ordering, where the best ordering maximizes
	the gain across all subtopics. The actual solution to this is NP-hard (references in [1]).

	[1] Clarke, C.L.A et. al. A comparative analysis of cascade measures for novelty and diversity. WSDM'11

	Here we misappropriate the best_entry_point value to store whether the assessment has been used or not.
	Can do this because TREC, which doesn't have bep does diversity, and INEX which has bep doesn't.
*/
void ANT_evaluation_intent_aware_normalised_discounted_cumulative_gain::set_lists(ANT_relevant_topic *relevant_topic_list, long long relevant_topic_list_length)
{
ANT_relevant_topic *got_top;
ANT_relevant_document *current_doc, *first_doc;
long long topic, subtopic, examine_docid, total_documents, relevant_documents, position, best_docid = -1, document;
double max_score, current_score = 0;

ANT_evaluation::set_lists(relevant_topic_list, relevant_topic_list_length);

ideal_gains = new double[relevant_topic_list_length];
for (topic = 0; topic < relevant_topic_list_length; topic++)
	{
	ideal_gains[topic] = 0;
	got_top = relevant_topic_list + topic;
	relevant_documents = total_documents = 0;
	for (subtopic = 0; subtopic < got_top->number_of_subtopics; subtopic++)
		{
		total_documents += got_top->subtopic_list[subtopic].number_of_documents;
		relevant_documents += got_top->subtopic_list[subtopic].number_of_relevant_documents;
		}

	first_doc = got_top->subtopic_list[0].document_list;

	/*
		We need to try each document for each position in the ideal ordering
	*/
	for (position = 0; position < relevant_documents && position < precision_point; position++)
		{
		examine_docid = -1;
		max_score = 0;
		current_doc = first_doc;

		while (current_doc - first_doc < total_documents)
			{
			/*
				If we haven't set the document to examine, then find the first one that hasn't been used
				otherwise find the next judgement for the document
			*/
			if (examine_docid == -1)
				{
				while (current_doc - first_doc < total_documents && current_doc->best_entry_point != UNUSED)
					current_doc++;

				/*
					If we didn't find a new document to consider then we are done for this ranking position
				*/
				if (current_doc - first_doc < total_documents)
					examine_docid = current_doc->docid;
				else
					break;
				}
			else
				while (current_doc - first_doc < total_documents && current_doc->docid != examine_docid)
					current_doc++;

			/*
				If we found an assessment, add it's contribution to this documents score, and mark it examined.
				If we already have a score, then adjust the relevant_documents to account for the same document
				being relevant for multiple subtopics
			*/
			if (current_doc - first_doc < total_documents)
				{
				current_score += gain(current_doc->relevant_characters, position) * discount(position);
				current_doc->best_entry_point = EXAMINED;
				current_doc++;
				}

			/*
				If we've reached the end of the document list, then we should check if this document
				is the best we've encountered so far for this position
			*/
			if (current_doc - first_doc >= total_documents)
				{
				if (current_score > max_score)
					{
					max_score = current_score;
					best_docid = examine_docid;
					}

				/*
					Reset for the next document
				*/
				current_doc = first_doc;
				examine_docid = -1;
				current_score = 0;
				}
			}

		/*
			Now we've found the best document for this position, then mark those assessments for that
			document as used, and reset the others
		*/
		for (current_doc = first_doc; current_doc - first_doc < total_documents; current_doc++)
			if (current_doc->docid == best_docid)
				current_doc->best_entry_point = USED;
			else if (current_doc->best_entry_point == EXAMINED)
				current_doc->best_entry_point = UNUSED;

		/*
			If the maximum score is 0, then we found no more relevant documents, this can happen if 
			some documents are relevant for more than one subtopics. If this happens our gain can't
			get bigger to stop bothering
		*/
		if (max_score == 0)
			break;

		ideal_gains[topic] += max_score;
		}
	}

/*
	Reset all the used flags, for other evaluations that use it.
*/
for (topic = 0; topic < relevant_topic_list_length; topic++)
	for (subtopic = 0; subtopic < relevant_topic_list[topic].number_of_subtopics; subtopic++)
		for (document = 0; document < relevant_topic_list[topic].subtopic_list[subtopic].number_of_documents; document++)
			relevant_topic_list[topic].subtopic_list[subtopic].document_list[document].best_entry_point = UNUSED;
}
