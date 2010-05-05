/*
	STEMMER_PAICE_HUSK.H
	--------------------
*/

#ifndef STEMMER_PAICE_HUSK_H_
#define STEMMER_PAICE_HUSK_H_

#include "stemmer.h"
#include "stem_paice_husk.h"

class ANT_stemmer_paice_husk : public ANT_stemmer
{
private:
	ANT_stem_paice_husk stemmer;
public:
	ANT_stemmer_paice_husk(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual size_t stem(char *term, char *destination)
	{
#ifdef ANT_HAS_PAICE_HUSK
	return stemmer.stem(term, destination);
#else
	return strlen(strcpy(destination, term));
#endif
	}
} ;

#endif  /* STEMMER_PAICE_HUSK_H_ */
