/*
	FOCUS_RESULT_FACTORY.C
	----------------------
*/
#include <new.h>
#include <stdio.h>
#include "focus_result_factory.h"
#include "focus_result.h"

/*
	ANT_FOCUS_RESULT_FACTORY::ANT_FOCUS_RESULT_FACTORY()
	----------------------------------------------------
*/
ANT_focus_result_factory::ANT_focus_result_factory(long max_results)
{
result = new (std::nothrow) ANT_focus_result[max_results];
result_length = result == NULL ? 0 : max_results;
result_used = 0;
}

/*
	ANT_FOCUS_RESULT_FACTORY::~ANT_FOCUS_RESULT_FACTORY()
	-----------------------------------------------------
*/
ANT_focus_result_factory::~ANT_focus_result_factory()
{
delete [] result;
}

/*
	ANT_FOCUS_RESULT_FACTORY::NEW_RESULT()
	--------------------------------------
*/
ANT_focus_result *ANT_focus_result_factory::new_result(void)
{
if (result_used < result_length)
	return result + result_used++;

return NULL;
}

/*
	ANT_FOCUS_RESULT_FACTORY::REWIND()
	----------------------------------
*/
void ANT_focus_result_factory::rewind(void)
{
result_used = 0;
}

