/*
	STEMMER_OTAGO.H
	---------------
*/
#ifndef STEMMER_OTAGO_H_
#define STEMMER_OTAGO_H_

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "stem_otago.h"

/*
	class ANT_STEMMER_OTAGO
	-----------------------
*/
class ANT_stemmer_otago : public ANT_stemmer, public ANT_stem_otago
{
public:
	ANT_stemmer_otago(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_stem_otago() {}
	virtual size_t stem(const char *term, char *destination) { return ANT_stem_otago::stem(term, destination); }
} ;

#endif  /* STEMMER_OTAGO_H_ */
