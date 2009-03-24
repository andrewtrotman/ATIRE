#ifndef __GA_STEMMER_H__
#define __GA_STEMMER_H__

#include <stdio.h>
#include "search_engine.h"
#include "btree_iterator.h"
#include "stemmer.h"
#include "ga_individual.h"

const int FIXED_SEARCH_LENGTH = SACROSANCT_CHARS;

class GA_stemmer : public ANT_stemmer {
private:
    char stemmed_prefix[MAX_TERM_LENGTH];
    GA_individual *individual;
	virtual char *get_next(char *);

public:
    void set_stemmer(GA_individual *individual) {this->individual = individual;}

	virtual size_t stem(char *term, char *destination);
    virtual char *first(char *);

    void print(FILE *);

    GA_stemmer(ANT_search_engine *search_engine) : ANT_stemmer(search_engine) {}
    virtual ~GA_stemmer() {}
} ;

#endif __GA_STEMMER_H__
