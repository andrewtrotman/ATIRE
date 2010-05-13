/*
	STEM_OTAGO.H
	------------
*/
#ifndef STEM_OTAGO_H_
#define STEM_OTAGO_H_

#include "stem.h"

/*
	class ANT_STEM_OTAGO
	--------------------
*/
class ANT_stem_otago : public ANT_stem
{
public:
	ANT_stem_otago() {}
	virtual ~ANT_stem_otago() {}
	virtual size_t stem(const char *term, char *destination);
	virtual char *name(void) { return "the Otago Stemmer"; }
} ;

#endif /* STEM_OTAGO_H_ */
