/*
	FOCUS_RESULTS_LIST.C
	--------------------
*/
#include <new>
#include <stdio.h>
#include "focus_results_list.h"
#include "focus_result.h"

/*
	ANT_FOCUS_RESULTS_LIST::ANT_FOCUS_RESULTS_LIST()
	------------------------------------------------
*/
ANT_focus_results_list::ANT_focus_results_list(long max_results)
{
result = new (std::nothrow) ANT_focus_result[max_results];
result_length = result == NULL ? 0 : max_results;
result_used = 0;
}

/*
	ANT_FOCUS_RESULTS_LIST::~ANT_FOCUS_RESULTS_LIST()
	-------------------------------------------------
*/
ANT_focus_results_list::~ANT_focus_results_list()
{
delete [] result;
}

/*
	ANT_FOCUS_RESULTS_LIST::NEW_RESULT()
	------------------------------------
*/
ANT_focus_result *ANT_focus_results_list::new_result(void)
{
if (result_used < result_length)
	return result + result_used++;

return NULL;
}

/*
	ANT_FOCUS_RESULTS_LIST::REWIND()
	--------------------------------
*/
void ANT_focus_results_list::rewind(void)
{
result_used = 0;
}

