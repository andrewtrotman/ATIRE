/*
	EVALUATION_INTENT_AWARE_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN.C
	---------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evaluation_discounted_cumulative_gain.h"
#include "evaluation_intent_aware_normalised_discounted_cumulative_gain.h"
#include "search_engine_result_iterator.h"
#include "relevant_topic.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"
#include "pragma.h"

/*
	ANT_EVALUATION_INTENT_AWARE_NORMALISED_DISCOUNTED_CUMULATIVE_GAIN::EVALUATE()
	-----------------------------------------------------------------------------
*/
double ANT_evaluation_intent_aware_normalised_discounted_cumulative_gain::evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic)
{
#if 1
double score, precision;
double *dcg, *subtopicGain;
long long i, current_subtopic;
ANT_relevant_topic *got_topic, key_topic;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
ANT_search_engine_result_iterator iterator;

key_topic.topic = topic;

got_topic = (ANT_relevant_topic *)bsearch(&key_topic, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);

if (got_topic == NULL)
	{
	printf("Didn't find %ld in qrels... no judgements?\n", topic);
	*valid = false;
	
	return 0;
	}

*valid = true;

dcg = new double[(size_t)precision_point];
subtopicGain = new double[(size_t)got_topic->number_of_subtopics];

for (i = 0; i < precision_point; i++)
	dcg[i] = 0;

for (i = 0; i < got_topic->number_of_subtopics; i++)
	subtopicGain[i] = 1;

key.topic = topic;
#ifdef FILENAME_INDEX
for (i = 0, key.docid = iterator.first(search_engine); key.docid != NULL && i < precision_point; key.docid = iterator.next(), i++)
#else
for (i = 0, key.docid = iterator.first(search_engine); key.docid >= 0 && i < precision_point; key.docid = iterator.next(), i++)
#endif
	{
	score = 0;
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
	dcg[i] = score * discount(i);
	}

for (i = 1; i < precision_point; i++)
	dcg[i] += dcg[i - 1];

precision = dcg[precision_point - 1];

delete [] subtopicGain;
delete [] dcg;

return precision / ideal_gains[got_topic - relevant_topic_list];

#else
double precision = 0;
long long current_subtopic;
ANT_relevant_topic *got_topic, key_topic;

key_topic.topic = topic;

got_topic = (ANT_relevant_topic *)bsearch(&key_topic, relevant_topic_list, (size_t)relevant_topic_list_length, sizeof(*relevant_topic_list), ANT_relevant_topic::compare);

for (current_subtopic = 0; current_subtopic < got_topic->number_of_subtopics; current_subtopic++)
	precision += ANT_evaluation_discounted_cumulative_gain::evaluate(search_engine, topic, (long)got_topic->subtopic_list[current_subtopic].subtopic);

printf("%f / %f\n", precision, ideal_gains[got_topic - relevant_topic_list]);
return precision / ideal_gains[got_topic - relevant_topic_list];
#endif

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
#ifdef FILENAME_INDEX
	char *examine_docid, *best_docid = NULL;
#else
	long long examine_docid, best_docid = -1;
#endif
ANT_relevant_topic *got_top;
ANT_relevant_document *current_doc, *first_doc;
long long topic, subtopic, total_documents, relevant_documents, position;
double max_score, current_score = 0;
double *subtopicGain = NULL;
long long this_subtopic_position;

ANT_evaluation::set_lists(relevant_topic_list, relevant_topic_list_length);

ideal_gains = new double[(size_t)relevant_topic_list_length];
for (topic = 0; topic < relevant_topic_list_length; topic++)
	{
	ideal_gains[topic] = 0;
	got_top = relevant_topic_list + topic;
	relevant_documents = total_documents = 0;

	delete [] subtopicGain;
	subtopicGain = new double[(size_t)got_top->number_of_subtopics];

	for (subtopic = 0; subtopic < got_top->number_of_subtopics; subtopic++)
		{
		subtopicGain[subtopic] = 1;
		total_documents += got_top->subtopic_list[subtopic].number_of_documents;
		relevant_documents += got_top->subtopic_list[subtopic].number_of_relevant_documents;
		}

	first_doc = got_top->subtopic_list[0].document_list;

	/*
		We need to try each document for each position in the ideal ordering
	*/
	for (position = 0; position < relevant_documents && position < precision_point; position++)
		{
#ifdef FILENAME_INDEX
		examine_docid = NULL;
#else
		examine_docid = -1;
#endif
		max_score = 0;
		current_doc = first_doc;

		while (current_doc->topic == got_top->topic)
			{
			/*
				If we haven't set the document to examine, then find the first one that hasn't been used
				otherwise find the next judgement for the document
			*/
#ifdef FILENAME_INDEX
			if (examine_docid == NULL)
#else
			if (examine_docid == -1)
#endif
				{
				while (current_doc->topic == got_top->topic && current_doc->best_entry_point != UNUSED)
					current_doc++;

				/*
					If we didn't find a new document to consider then we are done for this ranking position
				*/
				if (current_doc->topic == got_top->topic)
					examine_docid = current_doc->docid;
				else
					break;
				}
			else
#ifdef FILENAME_INDEX
				while (current_doc->topic == got_top->topic && strcmp(current_doc->docid, examine_docid) != 0)
#else
				while (current_doc->topic == got_top->topic && current_doc->docid != examine_docid)
#endif
					current_doc++;

			/*
				If we found an assessment, add it's contribution to this documents score, and mark it examined.
			*/
#ifdef FILENAME_INDEX
			if (current_doc->topic == got_top->topic && strcmp(current_doc->docid, examine_docid) == 0)
#else
			if (current_doc->topic == got_top->topic && current_doc->docid == examine_docid)
#endif
				{
				this_subtopic_position = got_top->subtopic_list[0].subtopic - current_doc->subtopic;
				if (current_doc->relevant_characters > 0)
					current_score += subtopicGain[this_subtopic_position];
				current_doc->best_entry_point = EXAMINED;
				}
			current_doc++;

			/*
				If we've reached the end of the document list, then we should check if this document
				is the best we've encountered so far for this position
			*/
			if (current_doc->topic != got_top->topic && current_doc->docid >= 0)
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
#ifdef FILENAME_INDEX
				examine_docid = NULL;
#else
				examine_docid = -1;
#endif
				current_score = 0;
				}
			}

		/*
			Now we've found the best document for this position, then mark those assessments for that
			document as used, and reset the others
		*/
		for (current_doc = first_doc; current_doc->topic == got_top->topic; current_doc++)
#ifdef FILENAME_INDEX
			if (strcmp(current_doc->docid, best_docid) == 0)
#else
			if (current_doc->docid == best_docid)
#endif
				{
				this_subtopic_position = got_top->subtopic_list[0].subtopic - current_doc->subtopic;
				// modify the subtopic gains now we've selected the document we're going to put in this position
				if (current_doc->relevant_characters > 0)
					subtopicGain[this_subtopic_position] *= (1.0 - alpha);
				current_doc->best_entry_point = USED;
				}
			else if (current_doc->best_entry_point == EXAMINED)
				current_doc->best_entry_point = UNUSED;

		/*
			If the maximum score is 0, then we found no more relevant documents, this can happen if 
			some documents are relevant for more than one subtopics. If this happens our gain can't
			get bigger to stop bothering
		*/
		if (max_score == 0)
			break;

		ideal_gains[topic] += max_score * discount(position);
		}
	/*
		Reset the flag for whether the document has been used, so that we can do multiple
		evaluations that need to use this flag
	*/
	for (current_doc = first_doc; current_doc->topic == got_top->topic; current_doc++)
		current_doc->best_entry_point = UNUSED;
	}
}
