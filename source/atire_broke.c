/*
	ATIRE_BROKE.C
	-------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "str.h"
#include "atire_broke.h"
#include "atire_broke_engine.h"
#include "atire_broker_param_block.h"
#include "atire_engine_result_set.h"
#include "fundamental_types.h"
#include "stats.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ATIRE_BROKE::ATIRE_BROKE()
	--------------------------
*/
ATIRE_broke::ATIRE_broke(ATIRE_broker_param_block *params) : ATIRE_engine(params)
{
long long current;

search_engine = NULL;
if (params->number_of_servers > 0)
	{
	search_engine = new ATIRE_broke_engine *[(size_t)(params->number_of_servers + 1)];
	for (current = 0; current < params->number_of_servers; current++)
		search_engine[current] = new ATIRE_broke_engine(params->servers[current]);
	search_engine[current] = NULL;
	}

if ((results_list = new (std::nothrow) ATIRE_engine_result_set) == NULL)
	exit(printf("Out of memory initialising the broker\n"));
}

/*
	ATIRE_BROKE::~ATIRE_BROKE()
	---------------------------
*/
ATIRE_broke::~ATIRE_broke()
{
ATIRE_broke_engine **current;

if (search_engine != NULL)
	for (current = search_engine; *current != NULL; current++)
		delete current;

delete [] search_engine;
delete results_list;
}

/*
	ATIRE_BROKE::LOAD_INDEX()
	-------------------------
*/
long ATIRE_broke::load_index(char *new_index, char *new_doclist, char **old_index, char **old_doclist)
{
*old_index = NULL;
*old_doclist = NULL;
return FALSE;			// always fail;
}

/*
	ATIRE_BROKE::DESCRIBE_INDEX()
	-----------------------------
*/
long ATIRE_broke::describe_index(char **old_index, char **old_doclist)
{
*old_index = "Broker";
*old_doclist = "Broker";
return TRUE;
}

/*
	ATIRE_BROKE::SEARCH()
	---------------------
*/
char *ATIRE_broke::search(char *query, long long first, long long page_length)
{
ANT_stats stats;
ATIRE_broke_engine **engine;
long long current, timer, hits, time_taken;
char *one_answer, *numhits;

//printf("BROKE:search(\"%s\", %lld, %lld)\n", query, first, page_length);

/*
	Not connected to anything
*/
if (search_engine == NULL || search_engine[0] == NULL)
	return NULL;

/*
	Only connected to one instance so my answer is that instance's answer
*/
if (search_engine[1] == NULL)
	return (*search_engine)->search(query, first, page_length);

/*
	Connected to many instances and so we need to merge
*/
timer = stats.start_timer();
current = 0;
hits = 0;
results_list->rewind();
for (engine = search_engine; *engine != NULL; engine++)
	{
	one_answer = (*engine)->search(query, 1, first + page_length);
	if ((numhits = strstr(one_answer, "<numhits>")) != NULL)
		hits += ANT_atoi64(numhits + 9);
	results_list->add(one_answer, ((long long)UINT32_MAX) * current++);
	}
time_taken = stats.time_to_milliseconds(stats.stop_timer(timer));

return results_list->serialise(query, hits, time_taken, first, page_length);
}

