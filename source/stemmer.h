/*
	STEMMER.H
	---------
*/

#ifndef STEMMER_H_
#define STEMMER_H_

#include "btree_iterator.h"
#include "ranking_function.h"

class ANT_stemmer : public ANT_btree_iterator
{
protected:
	char stemmed_search_term[MAX_TERM_LENGTH];
	char stemmed_index_term[MAX_TERM_LENGTH];
	size_t stemmed_search_term_length;

private:
	virtual char *get_next(char *);

public:
	ANT_stemmer(ANT_search_engine *engine) : ANT_btree_iterator(engine) {}
	virtual ~ANT_stemmer() {}

	virtual char *first(char *term);
	virtual char *next(void);

	virtual size_t stem(char *term, char *destination);		// returns the longest stem that could possibly match this stem given the algorithm (2 in the case of Porter).

    virtual long weight_terms(ANT_weighted_tf *term_weight, char *term);
} ;

#endif  /* STEMMER_H_ */
