/*
	STEMMER.H
	---------
*/

#ifndef __STEMMER_H__
#define __STEMMER_H__

#include "btree_iterator.h"

class ANT_stemmer : public ANT_btree_iterator
{
protected:
	char stemmed_search_term[MAX_TERM_LENGTH];
	char stemmed_index_term[MAX_TERM_LENGTH];
	long stemmed_search_term_length;

private:
	virtual char *get_next(char *);

public:
	ANT_stemmer(ANT_search_engine *engine) : ANT_btree_iterator(engine) {}
	virtual ~ANT_stemmer() {};

	virtual char *first(char *term);
	virtual char *next(void);

	virtual long stem(char *term, char *destination);
} ;

#endif __STEMMER_H__
