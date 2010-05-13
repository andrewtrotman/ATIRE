/*
	STEMMER_PORTER.H
	----------------
*/
#ifndef STEMMER_PORTER_H_
#define STEMMER_PORTER_H_

#include "stemmer.h"
#include "stem_porter.h"

/*
	class ANT_STEMMER_PORTER
	------------------------
*/
class ANT_stemmer_porter : public ANT_stemmer, public ANT_stem_porter
{
public:
	ANT_stemmer_porter(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_stem_porter() {}
	virtual size_t stem(const char *term, char *destination) { return ANT_stem_porter::stem(term, destination); }
	
} ;

#endif  /* STEMMER_PORTER_H_ */

