/*
	STEMMER_PORTER.H
	----------------
*/

#ifndef __STEMMER_PORTER_H__
#define __STEMMER_PORTER_H__

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"

class ANT_stemmer_porter : public ANT_stemmer
{
private:
	char workspace[MAX_TERM_LENGTH];

private:
	long isvowely(char *c) { return *c != '\0' && (ANT_isvowel(*c) || (*c == 'y' && !ANT_isvowel(*(c + 1)))); }
	long cvc(char *what) { return ((strchr("aeiouwxy", *what) == NULL) && (isvowely(what + 1)) && (!isvowely(what + 2))) ? 1 : 0; }
	long length(char *reversed);
	long has_vowel(char *what);

public:
	ANT_stemmer_porter(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual ~ANT_stemmer_porter() {}
	virtual long stem(char *term, char *destination);
} ;

#endif __STEMMER_PORTER_H__

