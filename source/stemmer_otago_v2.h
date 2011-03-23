/*
	STEMMER_OTAGO_V2.H
	------------------
*/
#ifndef STEMMER_OTAGO_V2_H_
#define STEMMER_OTAGO_V2_H_


#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "stem_otago_v2.h"

/*
	class ANT_STEMMER_OTAGO_V2
	--------------------------
*/
class ANT_stemmer_otago_v2 : public ANT_stemmer, public ANT_stem_otago_v2
{
public:
	ANT_stemmer_otago_v2(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_stem_otago_v2() {}
	virtual size_t stem(const char *term, char *destination) { return ANT_stem_otago_v2::stem(term, destination); }
} ;


#endif /* STEMMER_OTAGO_V2_H_ */
