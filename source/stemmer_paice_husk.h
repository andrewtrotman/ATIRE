/*
	STEMMER_PAICE_HUSK.H
	--------------------
*/

#ifndef STEMMER_PAICE_HUSK_H_
#define STEMMER_PAICE_HUSK_H_

#include "stemmer.h"
#include "paice_husk.h"

class ANT_stemmer_paice_husk : public ANT_stemmer
{
private:
	ANT_paice_husk stemmer;
public:
	ANT_stemmer_paice_husk(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual size_t stem(char *term, char *destination) { ANT_paice_husk_rule trule = stemmer.stem(term); strcpy(destination, trule.text); return 3; }	
} ;

#endif  /* STEMMER_PAICE_HUSK_H_ */
