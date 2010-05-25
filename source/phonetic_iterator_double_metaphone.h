/*
	PHONETIC_ITERATOR_DOUBLE_METAPHONE.H
	------------------------------------
*/
#ifndef PHONETIC_ITERATOR_DOUBLE_METAPHONE_H_
#define PHONETIC_ITERATOR_DOUBLE_METAPHONE_H_

#include <stdio.h>
#include "ctypes.h"
#include "stemmer.h"
#include "phonetic_double_metaphone.h"

/*
	class ANT_PHONETIC_ITERATOR_DOUBLE_METAPHONE
	--------------------------------------------
*/
class ANT_phonetic_iterator_double_metaphone : public ANT_stemmer, public ANT_phonetic_double_metaphone
{
public:
	ANT_phonetic_iterator_double_metaphone(ANT_search_engine *search_engine) : ANT_stemmer(search_engine), ANT_phonetic_double_metaphone() {}
	virtual size_t stem(const char *term, char *destination) { return ANT_phonetic_double_metaphone::stem(term, destination); }
} ;



#endif /* PHONETIC_ITERATOR_DOUBLE_METAPHONE_H_ */
