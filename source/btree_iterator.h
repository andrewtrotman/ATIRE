/*
	BTREE_ITERATOR.H
	----------------
*/
#ifndef __BTREE_ITERATOR_H__
#define __BTREE_ITERATOR_H__

class ANT_search_engine;
class ANT_search_engine_btree_leaf;

#define MAX_TERM_LENGTH 1024			// this should go and should be (somehow) replaced with search_engine->string_length_of_longest_term

class ANT_btree_iterator
{
private:
	unsigned char *btree_leaf_buffer;
	char keyword[MAX_TERM_LENGTH];
	long keyword_head_length;
	ANT_search_engine *search_engine;
	long node, leaf;

public:
	ANT_btree_iterator(ANT_search_engine *search_engine);
	virtual ~ANT_btree_iterator();

	virtual char *first(char *term);
	virtual char *next(void);
	ANT_search_engine_btree_leaf *get_postings_details(ANT_search_engine_btree_leaf *term_details);
} ;

#endif __BTREE_ITERATOR_H__
