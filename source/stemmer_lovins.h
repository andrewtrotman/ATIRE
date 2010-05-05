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
class ANT_stemmer_lovins : public ANT_stemmer
{
private:
	ANT_stem_lovins stemmer;

public:
	ANT_stemmer_lovins(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual size_t stem(char *term, char *destination) 
	{
#ifdef ANT_HAS_LOVINS
	stemmer.stem(term, destination);
	return 3;
#else
	return strlen(strcpy(destination, term));
#endif
	}
} ;

#endif  /* STEMMER_LOVINS_H_ */

