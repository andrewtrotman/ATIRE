/*
	ATIRE_BROKE.C
	-------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "pragma.h"
#include "fundamental_types.h"
#include "str.h"
#include "atire_broke.h"
#include "atire_broke_engine.h"
#include "atire_broker_param_block.h"
#include "atire_engine_result_set.h"
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
ATIRE_broke::ATIRE_broke(ATIRE_broker_param_block *params) : ATIRE_engine()
{
long long current;

search_engine = NULL;
if (params->number_of_servers > 0)
	{
	if ((search_engine = new (std::nothrow) ATIRE_broke_engine *[(size_t)(params->number_of_servers + 1)]) == NULL)
		exit(printf("Out of memory initialising the broker array\n"));
	for (current = 0; current < params->number_of_servers; current++)
		if ((search_engine[current] = new (std::nothrow) ATIRE_broke_engine(params->servers[current])) == NULL)
			exit(printf("Out of memory initialising each server\n"));
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
		delete *current;

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
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ATIRE_BROKE::DESCRIBE_INDEX()
	-----------------------------
*/
long ATIRE_broke::describe_index(char **old_index, char **old_doclist, long long *documents, long long *terms, long long *length_of_longest_document)
{
ATIRE_broke_engine **engine;
long long individual;
long status = TRUE;
char *got, *count;

*documents = 0;
*terms = 0;
*length_of_longest_document = 0;
*old_index = "Broker";
*old_doclist = "Broker";

/*
	Not connected to anything
*/
if (search_engine == NULL || search_engine[0] == NULL)
	return TRUE;

/*
	Ask each search engine in turn
*/
for (engine = search_engine; *engine != NULL; engine++)
	{
	if ((got = (*engine)->describe_index()) != NULL)
		{
		if ((count = strstr(got, "<docnum>")) != NULL)
			{
			/*
				How many documents in the instance
			*/
			individual = ANT_atoi64(count + 8);
			*documents += individual;
			}
		else if ((count = strstr(got, "<termnum>")) != NULL)
			{
			/*
				Length of the instance in (non-unique) terms, the term count
			*/
			individual = ANT_atoi64(count + 9);
			*terms += individual;
			}
		else if ((count = strstr(got, "<longestdoc>")) != NULL)
			{
			/*
				The length of the longest document in the collection so that we can allocate a buffer
			*/
			individual = ANT_atoi64(count + 12);
			if (*length_of_longest_document > individual)
				*length_of_longest_document = individual;
			}
		else
			status = FALSE;
		}
	else
		status = FALSE;

	delete [] got;
	}

return status;
}

/*
	ATIRE_BROKE::SEARCH()
	---------------------
*/
char *ATIRE_broke::search(char *query, long long first, long long page_length)
{
ANT_stats stats;
ATIRE_broke_engine **engine;
long long current, timer, hits, time_taken, virtual_document_id;
char *one_answer, *numhits;

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
virtual_document_id = 0;
for (engine = search_engine; *engine != NULL; engine++)
	{
	if ((one_answer = (*engine)->search(query, 1, first + page_length)) != NULL)
		{
		/*
			In the case of a failure by a search engine we'll get a NULL returned by the search request
			and consequently the result will be dropped from the final score (by this code not running)
			In the case of a failure by a broker we'll get an <ATIREerror> without a <numhits> which hence the brackets
		*/
		if ((numhits = strstr(one_answer, "<numhits>")) != NULL)
			{
			hits += ANT_atoi64(numhits + 9);
			results_list->add(one_answer, virtual_document_id);
			}
		}
	virtual_document_id += (*engine)->get_document_count();
	}
time_taken = stats.time_to_milliseconds(stats.stop_timer(timer));

return results_list->serialise(query, hits, time_taken, first, page_length);
}

/*
	ATIRE_BROKE::GET_DOCUMENT()
	---------------------------
*/
char *ATIRE_broke::get_document(char *document_buffer, long long *current_document_length, long long id)
{
ATIRE_broke_engine **engine;
long long top, base;

/*
	Not connected to anything
*/
if (search_engine == NULL || search_engine[0] == NULL)
	{
	*document_buffer = '\0';
	*current_document_length = 0;
	return NULL;
	}

/*
	Perform a linear search to find out which instance the document is in
	then go and get the document
*/
base = top = 0;
for (engine = search_engine; *engine != NULL; engine++)
	{
	top += (*engine)->get_document_count();
	if (top > id)
		(*engine)->get_document(document_buffer, current_document_length, id - base);
	base = top;
	}

return *document_buffer == '\0' ? NULL : document_buffer;
}

