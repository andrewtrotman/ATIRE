/*
	STEMMER_TERM_SIMILARITY.H
	-------------------------
*/
#ifndef STEMMER_TERM_SIMILARITY_H_
#define STEMMER_TERM_SIMILARITY_H_

#include "stemmer.h"

class ANT_search_engine;

/*
	class ANT_STEMMER_TERM_SIMILARITY 
	---------------------------------
*/
class ANT_stemmer_term_similarity : public ANT_stemmer
{
protected:
	ANT_stemmer *base_stemmer;
	long long buffer_length_squared;
	ANT_search_engine *search_engine;

	long *buffer;
	char term[MAX_TERM_LENGTH];
	long long document_frequency;

protected:
	long long fill_buffer_with_postings(char *term, long *buffer, long long *document_frequency);
	double buffer_similarity(char *b);

public:
	ANT_stemmer_term_similarity(ANT_search_engine *search_engine, ANT_stemmer *stemmer);
	virtual ~ANT_stemmer_term_similarity();

	virtual ANT_search_engine_btree_leaf *get_postings_details(ANT_search_engine_btree_leaf *term_details);
	virtual char *first(char *start);
	virtual char *next(void);
	double term_similarity(char *term1, char *term2);
} ;

#endif  /* STEMMER_TERM_SIMILARITY_H_ */
