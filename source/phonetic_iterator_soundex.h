/*
	PHONETIC_ITERATOR_SOUNDEX.H
	---------------------------
*/
#ifndef PHONETIC_ITERATOR_SOUNDEX_H_
#define PHONETIC_ITERATOR_SOUNDEX_H_

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "phonetic_soundex.h"

/*
	class ANT_PHONETIC_ITERATOR_SOUNDEX
	-----------------------------------
*/
class ANT_phonetic_iterator_soundex : public ANT_stemmer, public ANT_phonetic_soundex
{
public:
	ANT_phonetic_iterator_soundex(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_phonetic_soundex() {}
	virtual size_t stem(const char *term, char *destination) { return ANT_phonetic_soundex::stem(term, destination); }
} ;



#endif /* PHONETIC_ITERATOR_SOUNDEX_H_ */
