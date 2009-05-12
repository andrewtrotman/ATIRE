/*
	STEMMER_OTAGO.H
	---------------
*/
#ifndef __STEMMER_OTAGO_H__
#define __STEMMER_OTAGO_H__

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

#endif __STEMMER_OTAGO_H__
