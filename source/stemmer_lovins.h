/*
	STEMMER_LOVINS.H
	----------------
*/
#ifndef STEMMER_LOVINS_H_
#define STEMMER_LOVINS_H_

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "lovins.h"

/*
	class ANT_STEMMER_LOVINS 
	------------------------
*/
class ANT_stemmer_lovins : public ANT_stemmer
{
public:
	ANT_stemmer_lovins(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual size_t stem(char *term, char *destination) { return lovins_stem(term, destination), 3; }
	
} ;

#endif  /* STEMMER_LOVINS_H_ */

