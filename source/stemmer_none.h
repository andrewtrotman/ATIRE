/*
	STEMMER_NONE.H
	--------------
*/

#ifndef STEMMER_NONE_H_
#define STEMMER_NONE_H_

#include <string.h>
#include "stemmer.h"

/*
	class ANT_STEMMER_NONE
	----------------------
*/
class ANT_stemmer_none : public ANT_stemmer
{
public:
	ANT_stemmer_none(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
	virtual ~ANT_stemmer_none() {} 

	virtual char *first(char *term) { return ANT_btree_iterator::first(term); }
	virtual char *next(void) { return NULL; }

	virtual size_t stem(const char *term, char *dest) {  return strlen(strcpy(dest, term)); }
} ;


#endif  /* STEMMER_NONE_H_ */
