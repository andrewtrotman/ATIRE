/*
	STEMMER_LOVINS.H
	----------------
*/

#ifndef __STEMMER_LOVINS_H__
#define __STEMMER_LOVINS_H__

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "lovins.h"

class ANT_stemmer_lovins : public ANT_stemmer;
{
public:
	ANT_stemmer_porter(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual size_t stem(char *term, char *destination) { return Lovins_stem(term, destination); }
	
} ;


#endif __STEMMER_LOVINS_H__
