/*
	STEMMER_KROVETZ.H
	-----------------
*/
#ifndef STEMMER_KROVETZ_H_
#define STEMMER_KROVETZ_H_

#include "stemmer.h"
#include "stem_krovetz.h"

/*
	class ANT_STEMMER_KROVETZ
	-------------------------
*/
class ANT_stemmer_krovetz : public ANT_stemmer, public ANT_stem_krovetz
{
public:
	ANT_stemmer_krovetz(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_stem_krovetz() {}
	virtual size_t stem(const char *term, char *destination)
		{
		if (kstem_stem_tobuffer((char *)term, destination) <= 0)
			strcpy(destination, term);
		return 3;		// The shortest size of a head word
		}
} ;

#endif /* STEMMER_KROVETZ_H_ */
