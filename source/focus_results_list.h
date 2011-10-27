/*
	FOCUS_RESULTS_LIST.H
	--------------------
*/
#ifndef FOCUS_RESULTS_LIST_H_
#define FOCUS_RESULTS_LIST_H_

#include "focus_result.h"

/*
	class ANT_FOCUS_RESULTS_LIST
	----------------------------
*/
class ANT_focus_results_list
{
private:
	ANT_focus_result *result;
	long long result_length;
	long long result_used;

public:
	ANT_focus_results_list(long max_results);
	virtual ~ANT_focus_results_list();

	ANT_focus_result *new_result(void);
	void rewind(void);

	ANT_focus_result *get(long long index) { return index < result_used ? result + index : NULL; }
	long long get_list_length(void) { return result_used; }
} ;

#endif /* FOCUS_RESULTS_LIST_H_ */
