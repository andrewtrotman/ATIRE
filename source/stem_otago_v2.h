/*
	STEM_OTAGO_V2.H
	---------------
*/
#ifndef STEM_OTAGO_V2_H_
#define STEM_OTAGO_V2_H_

#include "stem.h"

/*
	class ANT_STEM_OTAGO_V2
	-----------------------
*/
class ANT_stem_otago_v2 : public ANT_stem
{
public:
	ANT_stem_otago_v2() {}
	virtual ~ANT_stem_otago_v2() {}
	virtual size_t stem(const char *term, char *destination);
	virtual char *name(void) { return "the Otago Stemmer (v2)"; }
} ;

#endif /* STEM_OTAGO_V2_H_ */

