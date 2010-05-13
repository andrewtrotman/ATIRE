/*
	STEMMER_LOVINS.H
	----------------
*/
#ifndef STEMMER_LOVINS_H_
#define STEMMER_LOVINS_H_

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "stem_lovins.h"
#include "pragma.h"

/*
	class ANT_STEMMER_LOVINS 
	------------------------
*/
#ifdef ANT_HAS_LOVINS
	class ANT_stemmer_lovins : public ANT_stemmer, public ANT_stem_lovins
	{
	public:
		ANT_stemmer_lovins(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_stem_lovins() {}
		virtual size_t stem(const char *term, char *destination) { return stemmer.stem(term, destination), 3; }
	} ;
#else
	class ANT_stemmer_lovins : public ANT_stemmer
	{
	public:
		ANT_stemmer_lovins(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
		virtual size_t stem(const char *term, char *destination) { return strlen(strcpy(destination, term)); }
	} ;
#endif

#endif  /* STEMMER_LOVINS_H_ */

