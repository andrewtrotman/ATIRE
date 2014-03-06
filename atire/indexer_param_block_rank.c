/*
	INDEXER_PARAM_BLOCK_RANK.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "str.h"
#include "indexer_param_block_rank.h"
#include "ranking_function_bm25.h"
#include "ranking_function_factory.h"

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::!ANT_INDEXER_PARAM_BLOCK_RANK()
	-------------------------------------------------------------
*/
ANT_indexer_param_block_rank::~ANT_indexer_param_block_rank()
{
delete [] field_name;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::ANT_INDEXER_PARAM_BLOCK_RANK()
	------------------------------------------------------------
*/
ANT_indexer_param_block_rank::ANT_indexer_param_block_rank()
{
feedback_ranking_function = ANT_ranking_function_factory_object::NONE;
ranking_function = ANT_ranking_function_factory_object::BM25;

feedback_p1 = p1 = ANT_RANKING_FUNCTION_BM25_DEFAULT_K1;
feedback_p2 = p2 = ANT_RANKING_FUNCTION_BM25_DEFAULT_B;
feedback_p3 = p3 = 0;
field_name = NULL;

//quantization = false;
//quantization_bits = 8;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_ONE_PARAMETER()
	-------------------------------------------------
*/
int ANT_indexer_param_block_rank::get_one_parameter(char *from, double *into)
{
if (from != NULL && *from == ':')
	{
	*into = atof(from + 1);
	return 1;
	}
else
	return 0;

//printf("[%s][%f]\n", from, *into);
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_TWO_PARAMETERS()
	--------------------------------------------------
	returns the number of parameters correctly parsed
*/
int ANT_indexer_param_block_rank::get_two_parameters(char *from, double *first, double *second)
{
char *ch = from;

if (get_one_parameter(ch, first) == 0)
	return 0;

if (get_one_parameter(ch = strchr(ch + 1, ':'), second) == 0)
	return 1;

return 2;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::GET_THREE_PARAMETERS()
	----------------------------------------------------
	returns the number of parameters correctly parsed
*/
int ANT_indexer_param_block_rank::get_three_parameters(char *from, double *first, double *second, double *third)
{
char *ch = from;

if (get_one_parameter(ch, first) == 0)
	return 0;

if (get_one_parameter(ch = strchr(ch + 1, ':'), second) == 0)
	return 1;

if (get_one_parameter(ch = strchr(ch + 1, ':'), third) == 0)
	return 2;

return 3;
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::SET_RANKER()
	------------------------------------------
	Parse the string to determine which ranking function (and parameters) to use.
	If the feedbacker parameter is true then set the ranking parameters for the feedbacker

	Returns non-zero on success, 0 on failure (e.g. no such ranking function or bad parameters).
	Failure leaves the parsed information in an undefined state.
*/
int ANT_indexer_param_block_rank::set_ranker(char *which, long feedbacker)
{
long ranker, name_length;
char *separator;

for (ranker = 0; ANT_list_of_rankers[ranker].id != ANT_ranking_function_factory_object::NONE; ranker++)
	{
	name_length = strlen(ANT_list_of_rankers[ranker].name);
	if ((strncmp(ANT_list_of_rankers[ranker].name, which, name_length) == 0) && (which[name_length] == '\0' || which[name_length] == ':'))
		{
		/*
			check for prefixing (because lmd is a substring of lmds)
		*/
		if (which[name_length] != '\0' && which[name_length] != ':')
			continue;

		if (feedbacker)
			{
			feedback_p1 = ANT_list_of_rankers[ranker].feedback_p1;
			feedback_p2 = ANT_list_of_rankers[ranker].feedback_p2;
			feedback_p3 = ANT_list_of_rankers[ranker].feedback_p3;

			if (ANT_list_of_rankers[ranker].parameter_count == 1)
				get_one_parameter(which + strlen(ANT_list_of_rankers[ranker].name), &feedback_p1);
			else if (ANT_list_of_rankers[ranker].parameter_count == 2)
				get_two_parameters(which + strlen(ANT_list_of_rankers[ranker].name), &feedback_p1, &feedback_p2);
			else
				get_three_parameters(which + strlen(ANT_list_of_rankers[ranker].name), &feedback_p1, &feedback_p2, &feedback_p3);
			//	printf("feedback_p1=%f feedback_p2=%f feedback_p3=%f\n", feedback_p1, feedback_p2, feedback_p3);
			this->feedback_ranking_function = ANT_list_of_rankers[ranker].id;

			return 1;
			}
		else
			{
			p1 = ANT_list_of_rankers[ranker].p1;
			p2 = ANT_list_of_rankers[ranker].p2;
			p3 = ANT_list_of_rankers[ranker].p3;

			if (ANT_list_of_rankers[ranker].parameter_count == 1)
				get_one_parameter(which + strlen(ANT_list_of_rankers[ranker].name), &p1);
			else if (ANT_list_of_rankers[ranker].parameter_count == 2)
				get_two_parameters(which + strlen(ANT_list_of_rankers[ranker].name), &p1, &p2);
			else
				get_three_parameters(which + strlen(ANT_list_of_rankers[ranker].name), &p1, &p2, &p3);

			// printf("p1=%f p2=%f p3=%f\n", p1, p2, p3);
			this->ranking_function = ANT_list_of_rankers[ranker].id;

			return 1;
			}
		}
	}

/*
	if we get here then we're not in the list of ranking functons, but we might be a special case
*/
if ((strncmp(which, "pregen:", strlen("pregen:")) == 0) && !feedbacker)
	{
	ranking_function = ANT_ranking_function_factory_object::PREGEN;
	delete [] field_name;
	field_name = strnew(which + strlen("pregen:"));

	p1 = 0;
	if ((separator = strchr(field_name, ':')) != NULL)
		p1 = *(separator + 1) == 'a';

	return 1;
	}

return 0; //Unknown Ranking Function
}

/*
	ANT_INDEXER_PARAM_BLOCK_RANK::HELP()
	------------------------------------
*/
void ANT_indexer_param_block_rank::help(char *title, char switch_char, long long allowable)
{
char buffer[1024];
long ranker;

printf("%s\n", title);
printf("%*.*s\n", (int)strlen(title), (int)strlen(title), "------------------------------------------");
printf("-%c[function]    Rank the result set using\n", switch_char);

for (ranker = 0; ANT_list_of_rankers[ranker].name != NULL; ranker++)
	if (allowable & ANT_list_of_rankers[ranker].flags)
		{
		if (ANT_list_of_rankers[ranker].parameter_count == 0)
			sprintf(buffer, "%s%30.30s", ANT_list_of_rankers[ranker].name, ANT_list_of_rankers[ranker].parameters);
		else
			sprintf(buffer, "%s:%s%30.30s", ANT_list_of_rankers[ranker].name, ANT_list_of_rankers[ranker].parameters, " ");
		printf("  %18.18s %s %s\n", buffer, ANT_list_of_rankers[ranker].description, isdefault(ANT_list_of_rankers[ranker].id));
		}

puts("");
}
