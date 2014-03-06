/*
	STEM_STEMMER.H
	--------------
*/
#ifndef STEM_STEMMER_H_
#define STEM_STEMMER_H_

#include "stem.h"

/*
	class ANT_STEM_STEMMER
	----------------------
*/
class ANT_stem_stemmer : public ANT_stem
{
private:
	ANT_stemmer *stemmer;
public:
	ANT_stem_stemmer(ANT_stemmer *stemmer) : ANT_stem() { this->stemmer = stemmer; }
	virtual ~ANT_stem_stemmer() {}

	virtual size_t stem(const char *term, char *destination) { return stemmer->stem(term, destination); }
	virtual char *name(void) { return "ANT_stemmer_stem"; }
	
} ;

#endif /* STEM_STEMMER_H_ */
