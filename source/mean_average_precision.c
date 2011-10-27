/*
	MEAN_AVERAGE_PRECISION.C
	------------------------
*/
#include <new>
#include <stdio.h>
#include <string.h>
#include "mean_average_precision.h"
#include "memory.h"
#include "relevant_topic.h"
#include "relevant_document.h"
#include "relevant_document_passage.h"
#include "search_engine.h"
#include "search_engine_accumulator.h"
#include "search_engine_result_iterator.h"
#include "focus_results_list.h"
#include "precision_recall.h"

/*
	ANT_MEAN_AVERAGE_PRECISION::ANT_MEAN_AVERAGE_PRECISION()
	--------------------------------------------------------
*/
ANT_mean_average_precision::ANT_mean_average_precision(ANT_memory *memory, ANT_relevant_document *relevance_list, long long relevance_list_length)
{
long long current, relevant_characters;
long long last_topic, current_topic, relevant_documents, nonrelevant_documents;

/*
	Take a copy of the relevance_list and sort it on topic then docid.
*/
this->memory = memory;
this->relevance_list = (ANT_relevant_document *)memory->malloc(sizeof(*relevance_list) * relevance_list_length);
this->relevance_list_length = relevance_list_length;
memcpy(this->relevance_list, relevance_list, (size_t)(sizeof(*relevance_list) * relevance_list_length));
qsort(this->relevance_list, (size_t)relevance_list_length, sizeof(*this->relevance_list), ANT_relevant_document::compare);

/*
	Find out how many topics there are in the list
*/
last_topic = -1;
topics_list_length = 0;
for (current = 0; current < relevance_list_length; current++)
	if (this->relevance_list[current].topic != last_topic)
		{
		last_topic = this->relevance_list[current].topic;
		topics_list_length++;
		}

/*
	Allocate memory for them and copy them in
*/
topics = (ANT_relevant_topic *)memory->malloc(sizeof(*topics) * topics_list_length);

last_topic = this->relevance_list[0].topic;	// init with the first topic
relevant_characters = this->relevance_list[0].relevant_characters;
relevant_documents = this->relevance_list[0].relevant_characters == 0 ? 0 : 1;
nonrelevant_documents = relevant_documents ? 0 : 1;
current_topic = 0;
for (current = 1; current < relevance_list_length; current++)
	if (this->relevance_list[current].topic != last_topic)
		{
		topics[current_topic].topic = last_topic;
		topics[current_topic].number_of_relevant_documents = relevant_documents;
		topics[current_topic].number_of_nonrelevant_documents = nonrelevant_documents;
		topics[current_topic].number_of_relevant_characters = relevant_characters;

		current_topic++;
		last_topic = this->relevance_list[current].topic;
		relevant_characters = this->relevance_list[current].relevant_characters;
		relevant_documents = this->relevance_list[current].relevant_characters == 0 ? 0 : 1;
		nonrelevant_documents = relevant_documents ? 0 : 1;
		}
	else if (this->relevance_list[current].relevant_characters == 0)
		nonrelevant_documents++;
	else
		{
		relevant_documents++;
		relevant_characters += this->relevance_list[current].relevant_characters;
		}

topics[current_topic].topic = last_topic;
topics[current_topic].number_of_relevant_documents = relevant_documents;
topics[current_topic].number_of_nonrelevant_documents = nonrelevant_documents;
topics[current_topic].number_of_relevant_characters = relevant_characters;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::SETUP()
	-----------------------------------
*/
ANT_relevant_topic *ANT_mean_average_precision::setup(long topic)
{
ANT_relevant_topic topic_key, *got;

topic_key.topic = topic;
got = (ANT_relevant_topic *)bsearch(&topic_key, topics, (size_t)topics_list_length, sizeof(topic_key), ANT_relevant_topic::compare);
if (got == NULL)
	fprintf(stderr, "Unexpected: Topic '%ld' not found in qrels - No relevant docs for query?\n", topic);

return got;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::AVERAGE_PRECISION()
	-----------------------------------------------
*/
double ANT_mean_average_precision::average_precision(long topic, ANT_search_engine *search_engine)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
long long current, found_and_relevant;
double precision;

if ((got = setup(topic)) == NULL)
	return 0;
if (got->number_of_relevant_documents == 0)
	return 0;

key.topic = topic;
precision = 0;
found_and_relevant = 0;

current = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0; key.docid = iterator.next())
	{
	current++;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		{
		/*
			At this point we have an assessment for the document, but it might have been assessed are irrelevant
		*/
		if (relevance_data->relevant_characters != 0)
			{
			/*
				At this point we know it was relevant
			*/
			found_and_relevant++;
			precision += (double)found_and_relevant / (double)current;
			}
		}
	}
return precision / got->number_of_relevant_documents;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::AVERAGE_INTERPOLATED_PRECISION()
	------------------------------------------------------------
	101 point average interpolated precision focused.
*/
double ANT_mean_average_precision::average_interpolated_precision(long topic, ANT_focus_results_list *results_list)
{
long long found_relevant_characters, found_characters, relevant_characters;
long current_focused_result, current_point;
ANT_focus_result *result;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
ANT_relevant_document_passage *which_passage;
ANT_precision_recall points[101];
double precision, recall;

if ((got = setup(topic)) == NULL)
	return 0;
if (got->number_of_relevant_documents == 0)
	return 0;

key.topic = topic;
found_relevant_characters = found_characters = relevant_characters = 0;

/*
	Initialise the 101 points
*/
for (current_point = 0; current_point < 101; current_point++)
	{
	points[current_point].recall = (double)current_point / 100.0;
	points[current_point].precision = 0.0;
	}

/*
	Compute precision and recall at each result
*/
for (current_focused_result = 0; current_focused_result < results_list->get_list_length(); current_focused_result++)
	{
	result = results_list->get(current_focused_result);
	key.docid = result->docid;
	found_characters += result->INEX_finish - result->INEX_start;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			{
			relevant_characters += relevance_data->relevant_characters;
			for (which_passage = relevance_data->passage_list; which_passage != NULL; which_passage = which_passage->next)
				found_relevant_characters += (long)MAgP_crossover(result->INEX_start, result->INEX_finish, which_passage->offset, which_passage->offset + which_passage->length);
			}
	precision = (double)found_relevant_characters / (double)found_characters;
	recall = (double)found_relevant_characters / (double)got->number_of_relevant_characters;

	/*
		Update the 101 points
	*/
	for (current_point = (long)(recall * 100); current_point >= 0; current_point--)
		if (precision > points[current_point].precision)
			points[current_point].precision = precision;
		else
			break;			// we only need to go back far enough that it can have an effect
	}

/*
	Compute the sum of the 101 interpolated precisions
*/
precision = 0;
for (current_point = 0; current_point < 101; current_point++)
	precision += points[current_point].precision;

/*
	Return the average interpolated precision
*/
return precision / 101.0;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::AVERAGE_GENERALISED_PRECISION_DOCUMENT()
	--------------------------------------------------------------------
	MAgP computed for whole documents
*/
double ANT_mean_average_precision::average_generalised_precision_document(long topic, ANT_search_engine *search_engine)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
long long current;
double precision, doc_precision, doc_recall, doc_f_score, found_and_relevant;
const double beta = 0.25;

if ((got = setup(topic)) == NULL)
	return 0;
if (got->number_of_relevant_documents == 0)
	return 0;

key.topic = topic;
precision = 0;
found_and_relevant = 0;

current = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0; key.docid = iterator.next())
	{
	current++;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		{
		/*
			We have an assessed document
		*/
		if (relevance_data->relevant_characters != 0)
			{
			/*
				And that document is assessed as relevant
			*/
			doc_precision = (double)relevance_data->relevant_characters / (double)relevance_data->document_length;
			doc_recall = 1.0;		// we retrieve the whole document so recall is 1.
			doc_f_score = (1.0 + beta * beta ) * (doc_precision  * doc_recall) / (beta * beta * doc_precision + doc_recall);
			found_and_relevant += doc_f_score;
			precision += (double)found_and_relevant / current;
			}
		}
	}

return precision / (double)got->number_of_relevant_documents;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::MAGP_CROSSOVER()
	--------------------------------------------
	Compute the crossover between the passage and the relevant passage and return it.
	The units are characters.
*/
long long ANT_mean_average_precision::MAgP_crossover(long long start, long long finish, long long relevant_start, long long relevant_finish)
{
if (finish <= relevant_start)
	return 0;			// the entire passage is before the relevant passage

if (start >= relevant_finish)
	return 0;			// the entire passage is after the relevant passage

if (start <= relevant_start && finish >= relevant_finish)
	return relevant_finish - relevant_start;		// the passage encloses the relevant passage

if (start <= relevant_start && finish <= relevant_finish)
	return finish - relevant_start;				// crosses over, start before relevant_start

if (start >= relevant_start && finish >= relevant_finish)
	return relevant_finish - start;				// crosses over, end after relevant_finish

printf("Cannot compute intersection between (%lld-%lld) and (%lld-%lld)\n", start, finish, relevant_start, relevant_finish);
return 0;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::AVERAGE_GENERALISED_PRECISION_FOCUSED()
	-------------------------------------------------------------------
	MAgP for focused results
*/
double ANT_mean_average_precision::average_generalised_precision_focused(long topic, ANT_focus_results_list *results_list)
{
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
ANT_relevant_document_passage *which_passage;
long long number_of_documents, current_focused_result;
long long previous_docid;
double precision, doc_precision, doc_recall, doc_f_score, found_and_relevant;
const double beta = 0.25;
ANT_focus_result *result;
long long found_relevant_bytes, found_bytes, relevant_bytes;

if ((got = setup(topic)) == NULL)
	return 0;
if (got->number_of_relevant_documents == 0)
	return 0;

key.topic = topic;
precision = 0;
found_and_relevant = 0;

number_of_documents = found_relevant_bytes  = 0;
found_bytes = relevant_bytes = 1;
doc_precision = doc_recall = 0;
previous_docid = -1;
result = NULL;
for (current_focused_result = 0; current_focused_result < results_list->get_list_length(); current_focused_result++)
	{
	result = results_list->get(current_focused_result);
	key.docid = result->docid;
	if (result->docid != previous_docid)
		{
		number_of_documents++;
		doc_precision = (double)found_relevant_bytes / (double)found_bytes;
		doc_recall = (double)found_relevant_bytes / (double)relevant_bytes;
		doc_f_score = (1.0 + beta * beta ) * (doc_precision  * doc_recall) / (beta * beta * doc_precision + doc_recall);
		found_and_relevant += doc_f_score;		  
		precision += (double)found_and_relevant / number_of_documents;
		found_bytes = found_relevant_bytes = relevant_bytes = 0;
		}
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			{
			relevant_bytes = relevance_data->relevant_characters;
			found_bytes += (long)(result->INEX_finish - result->INEX_start);
			for (which_passage = relevance_data->passage_list; which_passage != NULL; which_passage = which_passage->next)
				found_relevant_bytes += (long)MAgP_crossover(result->INEX_start, result->INEX_finish, which_passage->offset, which_passage->offset + which_passage->length);
			}
	previous_docid = result->docid;
	}

if (result != NULL && result->docid != previous_docid)
	{
	number_of_documents++;
	doc_precision = (double)found_relevant_bytes / (double)found_bytes;
	doc_recall = (double)found_relevant_bytes / (double)relevant_bytes;
	doc_f_score = (1.0 + beta * beta ) * (doc_precision  * doc_recall) / (beta * beta * doc_precision + doc_recall);
	found_and_relevant += doc_f_score;		  
	precision += (double)found_and_relevant / number_of_documents;
	found_bytes = found_relevant_bytes = relevant_bytes = 0;
	}

return precision / (double)got->number_of_relevant_documents;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::RANK_EFFECTIVENESS()
	------------------------------------------------
*/
double ANT_mean_average_precision::rank_effectiveness(long topic, ANT_search_engine *search_engine)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_topic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_nonrelevant, total_nonrelevant;
double precision;

if ((got = setup(topic)) == NULL)
	return 0;

if ((total_nonrelevant = got->number_of_nonrelevant_documents) == 0)
	return 1;	// topic has no non-relevant documents so they are all relevant so we score a perfect score

precision = 0;
found_and_nonrelevant = 0;
key.topic = topic;
for (key.docid = iterator.first(search_engine); key.docid >= 0; key.docid = iterator.next())
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters == 0)
			found_and_nonrelevant++;
		else
			precision += 1.0 - ((double)found_and_nonrelevant / (double)total_nonrelevant);

return precision / got->number_of_relevant_documents;
}

/*
	ANT_MEAN_AVERAGE_PRECISION::P_AT_N()
	------------------------------------
*/
double ANT_mean_average_precision::p_at_n(long topic, ANT_search_engine *search_engine, long precision_point_n)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_document key, *relevance_data;
long long found_and_relevant, found;

if (setup(topic) == NULL)
	return 0;

found = found_and_relevant = 0;
key.topic = topic;
for (key.docid = iterator.first(search_engine); key.docid >= 0; key.docid = iterator.next())
	{
	if (++found > precision_point_n)
		break;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			found_and_relevant++;
	}

return (double)found_and_relevant / (double)precision_point_n;		// we're computing p@n so divide by n
}

/*
	ANT_MEAN_AVERAGE_PRECISION::SUCCESS_AT_N()
	------------------------------------------
	This is an Otago "special" metric that returns 1 if the topic has at least one relevant
	document in the top n and 0 if it has none.  If all relevance is equal then it returns
	the success rate at point n.
*/
double ANT_mean_average_precision::success_at_n(long topic, ANT_search_engine *search_engine, long precision_point_n)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_document key, *relevance_data;
long long found_and_relevant, found;

if (setup(topic) == NULL)
	return 0;

found = found_and_relevant = 0;
key.topic = topic;
for (key.docid = iterator.first(search_engine); key.docid >= 0; key.docid = iterator.next())
	{
	if (++found > precision_point_n)
		break;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, relevance_list, (size_t)relevance_list_length, sizeof(*relevance_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters != 0)
			found_and_relevant++;
	}

return found_and_relevant == 0 ? 0.0 : 1.0;
}
