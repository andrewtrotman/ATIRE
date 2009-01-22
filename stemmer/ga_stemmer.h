#ifndef __GA_STEMMER_H__
#define __GA_STEMMER_H__

#include "search_engine.h"
#include "stemmer.h"
#include "ga_individual.h"

class GA_stemmer : public ANT_stemmer {
private:
    GA_individual *individual;
public:
    GA_stemmer(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
    virtual ~GA_stemmer() {}
	virtual long stem(char *term, char *destination);
} ;

#endif __GA_STEMMER_H__
