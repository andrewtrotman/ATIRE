/*
	STEMMER_SNOWBALL.H
	------------------
*/
#ifndef STEMMER_SNOWBALL_H_
#define STEMMER_SNOWBALL_H_

#ifdef ANT_HAS_SNOWBALL

#include "stemmer.h"
#include "stem_snowball.h"

/*
	class ANT_STEMMER_SNOWBALL
	--------------------------
*/
class ANT_stemmer_snowball : public ANT_stemmer, public ANT_stem_snowball
{
public:
	ANT_stemmer_snowball(ANT_search_engine *search_engine, char *algorithm, char *character_encoding = NULL) : ANT_stemmer(search_engine), ANT_stem_snowball(algorithm, character_encoding) {}
	virtual size_t stem(const char *term, char *destination) { return ANT_stem_snowball::stem(term, destination); }
} ;

#endif

#endif /* STEMMER_SNOWBALL_H_ */
