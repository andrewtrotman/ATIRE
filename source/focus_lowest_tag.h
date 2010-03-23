/*
	FOCUS_LOWEST_TAG.H
	------------------
*/
#ifndef FOCUS_LOWEST_TAG_H_
#define FOCUS_LOWEST_TAG_H_

#include "focus.h"
#include "parser.h"

/*
	class ANT_FOCUS_LOWEST_TAG
	--------------------------
*/
class ANT_focus_lowest_tag : public ANT_focus
{
private:
	ANT_parser parser;

public:
	ANT_focus_lowest_tag(ANT_focus_result_factory *result_factory) : ANT_focus(result_factory) {}
	virtual ~ANT_focus_lowest_tag() {}

	virtual ANT_focus_result *focus(unsigned char *document, long *results_length);
} ;

#endif /* FOCUS_LOWEST_TAG_H_ */
