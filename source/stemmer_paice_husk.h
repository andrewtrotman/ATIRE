/*
	STEMMER_PAICE_HUSK.H
	--------------------
*/

#ifndef STEMMER_PAICE_HUSK_H_
#define STEMMER_PAICE_HUSK_H_

#include "stemmer.h"
#include "stem_paice_husk.h"

/*
	class ANT_STEMMER_PAICE_HUSK
	----------------------------
*/
#ifdef ANT_HAS_PAICE_HUSK
	class ANT_stemmer_paice_husk : public ANT_stemmer, public ANT_stem_paice_husk
	{
	public:
		ANT_stemmer_paice_husk(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_stem_paice_husk() {}
		virtual size_t stem(const char *term, char *destination) { return ANT_stem_paice_husk::stem(term, destination); }
	} ;
#else
	class ANT_stemmer_paice_husk : public ANT_stemmer
	{
	public:
		ANT_stemmer_paice_husk(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
		virtual size_t stem(const char *term, char *destination) { return strlen(strcpy(destination, term)); }
	} ;
#endif

#endif  /* STEMMER_PAICE_HUSK_H_ */
