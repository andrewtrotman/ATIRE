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
#include "evaluation_bpref.h"
#include "evaluation_expected_reciprocal_rank.h"
#include "evaluation_mean_average_generalised_precision_document.h"
#include "evaluation_mean_average_precision.h"
#include "evaluation_normalised_discounted_cumulative_gain.h"
#include "evaluation_precision_at_n.h"
#include "evaluation_rank_effectiveness.h"
#include "evaluation_success_at_n.h"
#include "evaluation_intent_aware_mean_average_precision.h"
#include "maths.h"
#include "memory.h"
#include "relevant_document.h"
#include "relevant_topic.h"

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
printf("-%c[metric][@<n>]Score the result after <n> results (0=all) [default=0] set using:\n", switch_char);
puts("   MAP          Documents, Uninterpolated Mean Average Precision (TREC) [default]");
puts("   MAgP         Documents, Uninterpolated Mean Average generalised Precision (INEX)");
puts("   RankEff      Documents, Mean Rank Effectiveness (account for unassessed documents)");
puts("   bpref[-m]    Documents, bpref (account for unassessed documents) with minimum <m> in relevant (default=0)");
puts("   nDCG[t]      Documents, Normalised Discounted Cumulative Gain (using [t]rec's version)");
puts("   ERR[-m]      Documents, Expected Reciprocal Rank with maximum judgement <m> (default=4)");
puts("   P            Documents, Set-based precision");
puts("   S            Documents, Set-based success (1=found at least 1 relevant or 0=none)");
puts("   IA-MAP       Documents, Intent-Aware Uninterpolated Mean Average Precision");
puts("");
}

/*
	ANT_EVALUATOR::ADD_EVALUATION()
	-------------------------------
	Caveat Emptor: Be very careful about the order in which you add new evaluation functions, they all use strncmp
	because they can all be followed by @<n>, and that would be uncapturable by strcmp itself. The functions below
	should be added most specific to most general.
	
	For instance, if adding a new evaluation function beginning with P, then it would have to go _before_ the check
	for P@<n> otherwise it will just error out.
*/
long ANT_evaluator::add_evaluation(char *which)
{
ANT_evaluation **new_evaluators;
double *new_evaluations;
char *needle;
char **new_evaluation_names;

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

if (strncmp(which, "MAP", 3) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_mean_average_precision();
else if (strncmp(which, "MAgP", 4) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_mean_average_generalised_precision_document();
else if (strncmp(which, "RankEff", 7) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_rank_effectiveness();
else if (strncmp(which, "bpref", 5) == 0)
	{
	evaluators[number_evaluations_used] = new ANT_evaluation_bpref();
	/*
		now check for bpref-10 style
	*/
	if ((needle = strchr(which, '-')) != NULL)
		{
		needle++;
		if (ANT_isdigit(*needle))
			((ANT_evaluation_bpref *)evaluators[number_evaluations_used])->set_minimum(atol(needle));
		else
			exit(printf("<m> in bpref-<m> must be numeric!\n"));
		}
	}
else if (strncmp(which, "nDCG", 4) == 0)
	{
	evaluators[number_evaluations_used] = new ANT_evaluation_normalised_discounted_cumulative_gain();
	
	/*
		now check if the user specified trec-style nDCG
	*/
	if ((needle = strchr(which, 't')) == NULL)
		((ANT_evaluation_normalised_discounted_cumulative_gain *)evaluators[number_evaluations_used])->set_mode(false);
	}
else if (strncmp(which, "ERR", 3) == 0)
	{
	evaluators[number_evaluations_used] = new ANT_evaluation_expected_reciprocal_rank();
	/*
		now check for the maximum judgement parameter, specified by, e.g. ERR-4
	*/
	if ((needle = strchr(which, '-')) != NULL)
		{
		needle++;
		if (ANT_isdigit(*needle))
			((ANT_evaluation_expected_reciprocal_rank *)evaluators[number_evaluations_used])->set_maximum_judgement(atol(needle));
		else
			exit(printf("<m> in ERR-<m> must be numeric!\n"));
		}
	}
else if (strncmp(which, "P", 1) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_precision_at_n();
else if (strncmp(which, "S", 1) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_success_at_n();
else if (strncmp(which, "IA-MAP", 5) == 0)
	evaluators[number_evaluations_used] = new ANT_evaluation_intent_aware_mean_average_precision();
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
	evaluators[number_evaluations_used]->set_precision_point(-1);

evaluation_names[number_evaluations_used] = which;
number_evaluations_used++;

return number_evaluations_used;
}

/*
	ANT_EVALUATOR::INITIALISE()
	---------------------------
	Given a list of relevant_documents from the assessment reader, construct relevant_topics, then tell the evaluations to use them
*/
long ANT_evaluator::initialise(ANT_memory *memory, ANT_relevant_document *relevant_document_list, long long relevant_document_list_length)
{
long long current, evaluator, last_topic, current_topic, last_subtopic, current_subtopic, number_subtopics = 1;

/*
	Take a copy of the relevance_list and sort it on topic, subtopic, then docid.
*/
this->memory = memory;
this->relevant_document_list = (ANT_relevant_document *)memory->malloc(sizeof(*relevant_document_list) * relevant_document_list_length);
this->relevant_document_list_length = relevant_document_list_length;
memcpy(this->relevant_document_list, relevant_document_list, (size_t)(sizeof(*relevant_document_list) * relevant_document_list_length));
qsort(this->relevant_document_list, (size_t)relevant_document_list_length, sizeof(*this->relevant_document_list), ANT_relevant_document::compare);

/*
	Find out how many topics and subtopics there are in the list
*/
last_topic = last_subtopic = -1;
relevant_topic_list_length = 0;
for (current = 0; current < this->relevant_document_list_length; current++)
	{
	if (this->relevant_document_list[current].topic != last_topic)
		{
		last_topic = this->relevant_document_list[current].topic;
		relevant_topic_list_length++;
		}
	// subtopics, if they exist are 1-indexed, and if they don't this will always be 1
	number_subtopics = ANT_max(number_subtopics, this->relevant_document_list[current].subtopic);
	}

/*
	Allocate memory for them and copy them in
*/
relevant_topic_list = (ANT_relevant_topic *)memory->malloc(sizeof(*relevant_topic_list) * relevant_topic_list_length);

last_topic = last_subtopic = -1;
current_topic = current_subtopic = -1;

for (current = 0; current < relevant_document_list_length; current++)
	{
	// new document so set document-wide variables
	if (this->relevant_document_list[current].topic != last_topic)
		{
		current_topic++;
		last_topic = this->relevant_document_list[current].topic;
		last_subtopic = -1; // so if we have only 1 subtopic (non-diversity) then we always execute the stuff below
		
		relevant_topic_list[current_topic].topic = last_topic;
		relevant_topic_list[current_topic].number_of_subtopics = 0;
		relevant_topic_list[current_topic].subtopics = new long long[number_subtopics];
		relevant_topic_list[current_topic].number_of_relevant_documents = new long long[number_subtopics];
		relevant_topic_list[current_topic].number_of_nonrelevant_documents = new long long[number_subtopics];
		relevant_topic_list[current_topic].number_of_relevant_characters = new long long[number_subtopics];
		relevant_topic_list[current_topic].beginning_of_judgements = new long long[number_subtopics];
		}
	
	// new subtopic so initialise values for this subtopic
	if (this->relevant_document_list[current].subtopic != last_subtopic)
		{
		last_subtopic = this->relevant_document_list[current].subtopic;
		current_subtopic = relevant_topic_list[current_topic].number_of_subtopics;
		
		relevant_topic_list[current_topic].number_of_subtopics++;
		
		relevant_topic_list[current_topic].subtopics[current_subtopic] = last_subtopic;
		relevant_topic_list[current_topic].number_of_relevant_documents[current_subtopic] = 0;
		relevant_topic_list[current_topic].number_of_nonrelevant_documents[current_subtopic] = 0;
		relevant_topic_list[current_topic].number_of_relevant_characters[current_subtopic] = 0;
		relevant_topic_list[current_topic].beginning_of_judgements[current_subtopic] = current;
		}
	
	// new examine the number of characters to work out if it was relevant or not
	if (this->relevant_document_list[current].relevant_characters == 0)
		relevant_topic_list[current_topic].number_of_nonrelevant_documents[current_subtopic]++;
	else
		{
		relevant_topic_list[current_topic].number_of_relevant_documents[current_subtopic]++;
		relevant_topic_list[current_topic].number_of_relevant_characters[current_subtopic] += this->relevant_document_list[current].relevant_characters;
		}
	}
relevant_topic_list_length = current_topic + 1;

for (evaluator = 0; evaluator < number_evaluations_used; evaluator++)
	evaluators[evaluator]->set_lists(this->relevant_document_list, this->relevant_document_list_length, this->relevant_topic_list, this->relevant_topic_list_length);

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
