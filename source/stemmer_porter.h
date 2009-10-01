/*
	STEMMER_PORTER.H
	----------------
*/
#ifndef STEMMER_PORTER_H_
#define STEMMER_PORTER_H_

#include "stemmer.h"
#include "porter.h"

class ANT_stemmer_porter : public ANT_stemmer, public ANT_porter
{
public:
	ANT_stemmer_porter(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_porter() {}
	virtual size_t stem(char *term, char *destination) { return ANT_porter::stem(term, destination); }
	
} ;

#endif  /* STEMMER_PORTER_H_ */

