/*
	EVALUATOR.C
	-----------
*/
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ctypes.h"
#include "evaluator.h"
#include "evaluation.h"
#include "evaluation_binary_preference.h"
#include "evaluation_discounted_cumulative_gain.h"
#include "evaluation_expected_reciprocal_rank.h"
#include "evaluation_mean_average_generalised_precision_document.h"
#include "evaluation_mean_average_precision.h"
#include "evaluation_normalised_discounted_cumulative_gain.h"
#include "evaluation_precision_at_n.h"
#include "evaluation_rank_effectiveness.h"
#include "evaluation_success_at_n.h"
#include "evaluation_intent_aware_expected_reciprocal_rank.h"
#include "evaluation_intent_aware_mean_average_precision.h"
#include "evaluation_intent_aware_normalised_discounted_cumulative_gain.h"
#include "evaluation_intent_aware_precision_at_n.h"
#include "maths.h"
#include "memory.h"
#include "relevant_document.h"
#include "relevant_subtopic.h"
#include "relevant_topic.h"
#include "str.h"

/*
	ANT_EVALUATOR::ANT_EVALUATOR()
	------------------------------
*/
ANT_evaluator::ANT_evaluator()
{
memory = NULL;
relevant_topic_list = NULL;
relevant_topic_list_length = 0;
relevant_document_list = NULL;
relevant_document_list_length = 0;
is_initialised = false;

number_evaluations_used = 0;
number_evaluation_spaces = 1;

evaluators = new ANT_evaluation *[number_evaluation_spaces];
evaluations = new double[number_evaluation_spaces];
evaluation_names = new char *[number_evaluation_spaces];
}

/*
	ANT_EVALUATOR::~ANT_EVALUATOR()
	-------------------------------
*/
ANT_evaluator::~ANT_evaluator()
{
delete [] evaluators;
delete [] evaluations;
}

/*
	ANT_EVALUATOR::HELP()
	---------------------
*/
void ANT_evaluator::help(char *title, char switch_char)
{
printf("%s\n", title);
printf("%*.*s\n", (int)strlen(title), (int)strlen(title), "------------------------------------------");
printf("-%c[metric][@<n>]Score the result set after <n> results (0=all) [default=0] using:\n", switch_char);
//puts("   *            All non-intent-aware metrics, with @ set to 10 for P/S if not specified");
//puts("   IA-*         All intent-aware metrics");
puts("   MAP          Documents, Uninterpolated Mean Average Precision (TREC) [default]");
puts("   MAgP         Documents, Uninterpolated Mean Average generalised Precision (INEX)");
puts("   RankEff      Documents, Rank Effectiveness (account for unassessed documents)");
puts("   bpref[:m]    Documents, Binary Preference (account for unassessed documents) with minimum <m> in relevant (default=0)");
puts("   nDCG[t][:a]  Documents, Normalised Discounted Cumulative Gain (using [t]rec's version), with [a]lpha (alpha-nDCG) (default=0)");
puts("   ERR[:m]      Documents, Expected Reciprocal Rank with maximum judgement <m> (default=4)");
puts("   P            Documents, Set-based precision");
puts("   S            Documents, Set-based success (1=found at least 1 relevant or 0=none)");
puts("   IA-MAP       Documents, Intent-Aware Uninterpolated Mean Average Precision");
puts("   IA-nDCG[t][:a] Documents, Intent-Aware Normalised Discounted Cumulative Gain (using [t]rec's version) with [a]lpha (default=0)");
puts("   IA-ERR[:m]   Documents, Intent-Aware Expected Reciprocal Rank with maximum judgement <m> (default=4)");
puts("   IA-P         Documents, Intent-Aware Precision");
puts("");
}

/*
	ANT_EVALUATOR::GET_PARAMETER()
	------------------------------
*/
long ANT_evaluator::get_parameter(char *from, double *into)
{
if (from != NULL)
	if (*from == ':')
		{
		*into = atof(from + 1);
		return true; // successfully got a parameter
		}
	else
		return false; // failed to get a parameter
return true; // successfully didn't get a parameter
}

/*
	ANT_EVALUATOR::ADD_EVALUATION()
	-------------------------------
	Caveat Emptor: Be very careful about the order in which you add new evaluation functions, they all use strncmp
	because they can all be followed by @<n>, and that would be uncapturable by strcmp itself. The functions below
	should be added most specific to most general.
	
	For instance, if adding a new evaluation function beginning with P, then it would have to go _before_ the check
	for P@<n> otherwise it will get the wrong functions.
*/
long ANT_evaluator::add_evaluation(char *which)
{
ANT_evaluation **new_evaluators;
double *new_evaluations;
char **new_evaluation_names;
char *needle, *eval, *buf = new char[20];

/*
	Make sure we have enough space for our evaluators
*/
if (number_evaluations_used == number_evaluation_spaces)
	{
	new_evaluators = new ANT_evaluation *[number_evaluation_spaces * 2];
	new_evaluations = new double[number_evaluation_spaces * 2];
	new_evaluation_names = new char *[number_evaluation_spaces * 2];
	memcpy(new_evaluators, evaluators, number_evaluations_used * sizeof(*evaluators));
	memcpy(new_evaluations, evaluations, number_evaluations_used * sizeof(*evaluations));
	memcpy(new_evaluation_names, evaluation_names, number_evaluations_used * sizeof(*evaluation_names));
	delete [] evaluators;
	delete [] evaluations;
	delete [] evaluation_names;
	evaluators = new_evaluators;
	evaluations = new_evaluations;
	evaluation_names = new_evaluation_names;
	number_evaluation_spaces *= 2;
	}

if (strncmp(which, "*", 1) == 0)
	{
	needle = strchr(which, '@');
	eval = (strcpy(buf, "MAP"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "MAgP"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "RankEff"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "bpref"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "nDCG"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "nDCGt"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "ERR"), needle != NULL ? strcat(buf, needle) : buf);
	add_evaluation(eval);
	eval = (strcpy(buf, "P"), strcat(buf, needle != NULL ? needle : "@10"));
	add_evaluation(eval);
	eval = (strcpy(buf, "S"), strcat(buf, needle != NULL ? needle : "@10"));
	add_evaluation(eval);
	return number_evaluations_used;
	}

if (strncmp(which, "RankEff", 7) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_rank_effectiveness;
else if (strncmp(which, "bpref", 5) == 0)
	{
	evaluators[number_evaluations_used] = new ANT_evaluation_binary_preference;
	if (!get_parameter(strchr(which, ':'), &((ANT_evaluation_binary_preference *)evaluators[number_evaluations_used])->minimum))
		exit(puts("<m> in bpref:<m> must be numeric!"));
	}
else if (strncmp(which, "nDCG", 4) == 0)
	{
	evaluators[number_evaluations_used] = new ANT_evaluation_normalised_discounted_cumulative_gain;
	((ANT_evaluation_normalised_discounted_cumulative_gain *)evaluators[number_evaluations_used])->set_mode(*(which + 4) == 't');
	if (!get_parameter(strchr(which, ':'), &((ANT_evaluation_normalised_discounted_cumulative_gain *)evaluators[number_evaluations_used])->alpha))
		exit(puts("<a> in nDCG[t]:<a> must be numeric!"));
	}
else if (strncmp(which, "MAgP", 4) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_mean_average_generalised_precision_document;
else if (strncmp(which, "MAP", 3) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_mean_average_precision;
else if (strncmp(which, "ERR", 3) == 0)
	{
	evaluators[number_evaluations_used] = new ANT_evaluation_expected_reciprocal_rank;
	if (!get_parameter(strchr(which, ':'), &((ANT_evaluation_expected_reciprocal_rank *)evaluators[number_evaluations_used])->maximum_judgement))
		exit(puts("<m> in ERR:<m> must be numeric!"));
	}
else if (strncmp(which, "IA-", 3) == 0)
	{
	needle = which + 3;
	if (*needle == '*')
		{
		needle = strchr(which, '@');
		eval = (strcpy(buf, "IA-MAP"), needle != NULL ? strcat(buf, needle) : buf);
		add_evaluation(eval);
		eval = (strcpy(buf, "IA-nDCG"), needle != NULL ? strcat(buf, needle) : buf);
		add_evaluation(eval);
		eval = (strcpy(buf, "IA-nDCGt"), needle != NULL ? strcat(buf, needle) : buf);
		add_evaluation(eval);
		eval = (strcpy(buf, "IA-ERR"), needle != NULL ? strcat(buf, needle) : buf);
		add_evaluation(eval);
		eval = (strcpy(buf, "IA-P"), needle != NULL ? strcat(buf, needle) : strcat(buf, "@10"));
		add_evaluation(eval);
		return number_evaluations_used;
		}
	
	if (strncmp(needle, "MAP", 3) == 0)
		evaluators[number_evaluations_used] = new ANT_evaluation_intent_aware_mean_average_precision;
	else if (strncmp(needle, "nDCG", 4) == 0)
		{
		evaluators[number_evaluations_used] = new ANT_evaluation_intent_aware_normalised_discounted_cumulative_gain;
		((ANT_evaluation_intent_aware_normalised_discounted_cumulative_gain *)evaluators[number_evaluations_used])->set_mode(*(which + 7) == 't');
		if (!get_parameter(strchr(needle, ':'), &((ANT_evaluation_intent_aware_normalised_discounted_cumulative_gain *)evaluators[number_evaluations_used])->alpha))
			exit(puts("<a> in IA-nDCG[t]:<a> must be numeric!"));
		}
	else if (strncmp(needle, "ERR", 3) == 0)
		{
		evaluators[number_evaluations_used] = new ANT_evaluation_intent_aware_expected_reciprocal_rank;
		if (!get_parameter(strchr(needle, ':'), &((ANT_evaluation_intent_aware_expected_reciprocal_rank *)evaluators[number_evaluations_used])->maximum_judgement))
			exit(puts("<m> in IA-ERR:<m> must be numeric!"));
		}
	else if (strncmp(needle, "P", 1) == 0)
		evaluators[number_evaluations_used] = new ANT_evaluation_intent_aware_precision_at_n;
	else
		exit(printf("Unknown metric: '%s'\n", which));
	}
else if (strncmp(which, "P", 1) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_precision_at_n;
else if (strncmp(which, "S", 1) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_success_at_n;
else
	exit(printf("Unknown metric: '%s'\n", which));

if ((needle = strchr(which, '@')) != NULL)
	{
	needle++;
	if (ANT_isdigit(*needle))
		evaluators[number_evaluations_used]->set_precision_point(atoll(needle));
	else
		exit(puts("<n> in *@<n> must be numeric!"));
	}
else
	evaluators[number_evaluations_used]->set_precision_point(LLONG_MAX);

evaluation_names[number_evaluations_used] = strnew(which);
number_evaluations_used++;

return number_evaluations_used;
}

/*
	ANT_EVALUATOR::INITIALISE()
	---------------------------
	At this point the best we can get is a list of individual judgements, because the qrels file might be a higgldy piggldy mess.
	So from the individual assessments we should construct the topic/subtopic/document hierarchy.
*/
long long ANT_evaluator::initialise(ANT_memory *memory, ANT_relevant_document *relevant_document_list, long long relevant_document_list_length)
{
long long current, relevant_documents, nonrelevant_documents, relevant_characters;
long long last_topic, current_topic, changed_topic;
long long last_subtopic, current_subtopic, number_of_subtopics;

/*
 Take a copy of the relevance_list and sort it on topic, subtopic, then docid.^M
*/
this->memory = memory;
this->relevant_document_list = (ANT_relevant_document *)memory->malloc(sizeof(*relevant_document_list) * relevant_document_list_length);
this->relevant_document_list_length = relevant_document_list_length;
memcpy(this->relevant_document_list, relevant_document_list, (size_t)(sizeof(*relevant_document_list) * relevant_document_list_length));
qsort(this->relevant_document_list, (size_t)relevant_document_list_length, sizeof(*this->relevant_document_list), ANT_relevant_document::compare);

/*
	Find out how many topics are in the list
*/
last_topic = -1;
relevant_topic_list_length = 0;
for (current = 0; current < this->relevant_document_list_length; current++)
	if (this->relevant_document_list[current].topic != last_topic)
		{
		last_topic = this->relevant_document_list[current].topic;
		relevant_topic_list_length++;
		}
relevant_topic_list = (ANT_relevant_topic *)memory->malloc(sizeof(*relevant_topic_list) * relevant_topic_list_length);

/*
	Now for each topic, find the number of subtopics and allocate space for them, + 1 because
	0-based vs. 1-based subtopic indexing

	0-based: will get the correct number
	1-based: will get an extra 1 that won't be found when doing evaluations,
					 so it does not matter
*/
last_topic = this->relevant_document_list[0].topic;
last_subtopic = this->relevant_document_list[0].subtopic;
number_of_subtopics = last_subtopic + 1;
current_topic = 0;
for (current = 1; current < this->relevant_document_list_length; current++)
	{
	/*
		New topic, so update the subtopic stats
	*/
	if (this->relevant_document_list[current].topic != last_topic)
		{
		this->relevant_topic_list[current_topic].topic = last_topic;
		this->relevant_topic_list[current_topic].number_of_subtopics = number_of_subtopics;
		this->relevant_topic_list[current_topic].subtopic_list = (ANT_relevant_subtopic *)memory->malloc(sizeof(*this->relevant_topic_list[current_topic].subtopic_list) * number_of_subtopics);
		
		last_topic = this->relevant_document_list[current].topic;
		current_topic++;
		
		/*
			By definition if we have a new topic, we _must_ have a new subtopic
			We have subtopics sorted in descending order, so the first subtopic will
			have the biggest subtopic id

			0-based: will get the correct number
			1-based: will get an extra 1 that won't be found when doing evaluations,
			         so it does not matter
		*/
		number_of_subtopics = this->relevant_document_list[current].subtopic + 1;
		last_subtopic = -1;
		}
	}
this->relevant_topic_list[current_topic].topic = last_topic;
this->relevant_topic_list[current_topic].number_of_subtopics = number_of_subtopics;
this->relevant_topic_list[current_topic].subtopic_list = (ANT_relevant_subtopic *)memory->malloc(sizeof(*this->relevant_topic_list[current_topic].subtopic_list) * number_of_subtopics);

/*
	Now for each topic/subtopic sort out the values for that subtopic
*/
last_topic = this->relevant_document_list[0].topic;
last_subtopic = this->relevant_document_list[0].subtopic;
current_topic = current_subtopic = 0;
changed_topic = false;
relevant_characters = this->relevant_document_list[0].relevant_characters;
relevant_documents = relevant_characters > 0 ? 1 : 0;
nonrelevant_documents = relevant_documents ? 0 : 1;

this->relevant_topic_list[0].subtopic_list[0].document_list = this->relevant_document_list;
for (current = 1; current < this->relevant_document_list_length; current++)
	{
	changed_topic = this->relevant_document_list[current].topic != last_topic;

	/*
		If we have a new topic, or a new subtopic, we can update the values
	*/
	if (changed_topic || this->relevant_document_list[current].subtopic != last_subtopic)
		{
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].topic = last_topic;
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].subtopic = last_subtopic;
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_documents = relevant_documents + nonrelevant_documents;
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_relevant_documents = relevant_documents;
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_nonrelevant_documents = nonrelevant_documents;
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_relevant_characters = relevant_characters;
		
		/*
			If we changed topic, then we want to change the topic level insert, otherwise keep going on subtopics
		*/
		if (changed_topic)
			{
			current_topic++;
			last_topic = this->relevant_document_list[current].topic;
			current_subtopic = 0;
			}
		else
			current_subtopic++;

		/*
			The document judgements point to this list
		*/
		this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].document_list = this->relevant_document_list + current;
		
		/*
			Reset the counts for this subtopic, and update the last subtopic seen
		*/
		nonrelevant_documents = relevant_documents = relevant_characters = 0;
		last_subtopic = this->relevant_document_list[current].subtopic;
		}
	
	if (this->relevant_document_list[current].relevant_characters <= 0)
		nonrelevant_documents++;
	else
		{
		relevant_documents++;
		relevant_characters += this->relevant_document_list[current].relevant_characters;
		}
	}
this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].topic = last_topic;
this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].subtopic = last_subtopic;
this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_documents = relevant_documents + nonrelevant_documents;
this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_relevant_documents = relevant_documents;
this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_nonrelevant_documents = nonrelevant_documents;
this->relevant_topic_list[current_topic].subtopic_list[current_subtopic].number_of_relevant_characters = relevant_characters;

/*
	Tell each of the evaluation methods about the lists
*/
for (current = 0; current < number_evaluations_used; current++)
	evaluators[current]->set_lists(this->relevant_topic_list, this->relevant_topic_list_length);

is_initialised = true;

return relevant_topic_list_length;
}

/*
	ANT_EVALUATOR::PERFORM_EVALUATION()
	-----------------------------------
*/
double *ANT_evaluator::perform_evaluation(ANT_search_engine *engine, long topic)
{
long evaluator;

for (evaluator = 0; evaluator < number_evaluations_used; evaluator++)
	evaluations[evaluator] = evaluators[evaluator]->evaluate(engine, topic);

return evaluations;
}
