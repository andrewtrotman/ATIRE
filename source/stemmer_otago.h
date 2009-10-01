/*
	STEMMER_OTAGO.H
	---------------
*/
#ifndef STEMMER_OTAGO_H_
#define STEMMER_OTAGO_H_

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "learned_wikipedia_stem.h"

/*
	class ANT_STEMMER_OTAGO
	-----------------------
*/
class ANT_stemmer_otago : public ANT_stemmer, public ANT_learned_wikipedia_stem
{
public:
	ANT_stemmer_otago(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_learned_wikipedia_stem() {}
	virtual size_t stem(char *term, char *destination) { return ANT_learned_wikipedia_stem::stem(term, destination); }
} ;

#endif  /* STEMMER_OTAGO_H_ */
