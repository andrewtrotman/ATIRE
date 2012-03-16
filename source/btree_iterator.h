/*
	BTREE_ITERATOR.H
	----------------
*/
#ifndef BTREE_ITERATOR_H_
#define BTREE_ITERATOR_H_

#ifndef _MSC_VER
#include <stddef.h>
#endif

class ANT_search_engine;
class ANT_search_engine_btree_leaf;

#define MAX_TERM_LENGTH (20 * 1024)			// this should go and should be (somehow) replaced with search_engine->string_length_of_longest_term

class ANT_btree_iterator
{
private:
	unsigned char *btree_leaf_buffer;
	char keyword[MAX_TERM_LENGTH];
	size_t keyword_head_length;
	ANT_search_engine *search_engine;
	long node, leaf;

public:
	ANT_btree_iterator(ANT_search_engine *search_engine);
	virtual ~ANT_btree_iterator();

	virtual char *first(char *term);
	virtual char *next(void);
	virtual ANT_search_engine_btree_leaf *get_postings_details(ANT_search_engine_btree_leaf *term_details);

	static const long LEAF_SIZE;
} ;

#endif  /* BTREE_ITERATOR_H_ */
