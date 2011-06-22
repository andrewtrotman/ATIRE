/*
	ATIRE_BROKE.C
	-------------
*/
#include <stdio.h>
#include "atire_broke.h"
#include "atire_broke_engine.h"
#include "atire_broker_param_block.h"

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
long ATIRE_broke::search(char *query, long long first, long long last)
{
ATIRE_broke_engine **engine;

printf("BROKE:search(\"%s\", %lld, %lld)\n", query, first, last);
if (search_engine != NULL)
	{
	for (engine = search_engine; *engine != NULL; engine++)
		(*engine)->search(query, first, last);
	}

return TRUE;
}

