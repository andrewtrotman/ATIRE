/*
	STEMMER_STEM.H
	--------------
*/
#ifndef STEMMER_STEM_H_
#define STEMMER_STEM_H_

#include "stemmer.h"
#include "stem.h"

/*
	class ANT_STEMMER_STEM
	----------------------
*/
class ANT_stemmer_stem : public ANT_stemmer
{
private:
	ANT_stem *stemmer;

public:
	ANT_stemmer_stem(ANT_search_engine *search_engine, ANT_stem *stemmer) : ANT_stemmer(search_engine) { this->stemmer = stemmer; }
	virtual size_t stem(const char *term, char *destination) { return stemmer->stem((char *)term, destination); }
} ;

#endif /* STEMMER_STEM_H_ */
