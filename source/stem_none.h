/*
	STEM_NONE.H
	-----------
*/
#ifndef STEM_NONE_H_
#define STEM_NONE_H_

#include <string.h>
#include "stem.h"


/*
	class ANT_STEM_NONE
	-------------------
*/
class ANT_stem_none : public ANT_stem
{
public:
	ANT_stem_none() {}
	virtual ~ANT_stem_none() {}
	virtual size_t stem(const char *term, char *destination) { return strlen(strcpy(destination, term)); }
	virtual char *name(void) { return "Nothing"; }
} ;

#endif /* STEM_NONE_H_ */
