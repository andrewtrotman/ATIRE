/*
	STEMMER_TERM_SIMILARITY.H
	----------------
*/
#ifndef __STEMMER_TERM_SIMILARITY_H__
#define __STEMMER_LOVINS_H__

#include "stemmer.h"

class ANT_search_engine;

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
	long document_frequency;

private:
	long long fill_buffer_with_postings(char *term, long *buffer, long *document_frequency);
	double buffer_similarity(char *b);
	double term_similarity(char *term1, char *term2);

public:
	ANT_stemmer_term_similarity(ANT_search_engine *search_engine, ANT_stemmer *stemmer, double threshold);
	virtual ~ANT_stemmer_term_similarity();

	virtual char *first(char *start);
	virtual char *next(void);
} ;

#endif __STEMMER_TERM_SIMILARITY_H__
