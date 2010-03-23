/*
	FOCUS_RESULT_FACTORY.H
	----------------------
*/
#ifndef FOCUS_RESULT_FACTORY_H_
#define FOCUS_RESULT_FACTORY_H_

class ANT_focus_result;

/*
	class ANT_FOCUS_RESULT_FACTORY
	------------------------------
*/
class ANT_focus_result_factory
{
private:
	ANT_focus_result *result;
	long result_length;
	long result_used;

public:
	ANT_focus_result_factory(long max_results);
	virtual ~ANT_focus_result_factory();

	ANT_focus_result *new_result(void);
	void rewind(void);
} ;

#endif /* FOCUS_RESULT_FACTORY_H_ */

