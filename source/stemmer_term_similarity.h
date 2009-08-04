/*
	STEMMER_TERM_SIMILARITY.H
	----------------
*/
#ifndef __STEMMER_TERM_SIMILARITY_H__
#define __STEMMER_LOVINS_H__

#include <stdio.h>
#include <stdlib.h>
#include "ctypes.h"
#include "stemmer.h"
#include "search_engine.h"
#include "btree_iterator.h"

/*
	class ANT_STEMMER_TERM_SIMILARITY 
	---------------------------------
*/
class ANT_stemmer_term_similarity : public ANT_stemmer
{
private:
    ANT_stemmer *base_stemmer;
    double threshold;
    long long buffer_length_squared;
    ANT_search_engine *search_engine;

	long *buffer;
    char term[MAX_TERM_LENGTH];

private:
	long long fill_buffer_with_postings(char *term, long *buffer);
	double buffer_similarity(char *a, char *b);
	double term_similarity(char *term1, char *term2);

public:
	ANT_stemmer_term_similarity(ANT_search_engine *search_engine, ANT_stemmer *stemmer, double threshold) : ANT_stemmer(search_engine) 
    {
        this->buffer = (long *)malloc(sizeof *buffer * search_engine->document_count());
        this->search_engine = search_engine;
        this->base_stemmer = stemmer;
        this->threshold = threshold;
    }

    virtual char *first(char *);
	virtual char *next();

    virtual ~ANT_stemmer_term_similarity() { delete base_stemmer; free(buffer); }
} ;

#endif __STEMMER_TERM_SIMILARITY_H__
