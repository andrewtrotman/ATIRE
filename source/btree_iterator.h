/*
	BTREE_ITERATOR.H
	----------------
*/

#ifndef __BTREE_ITERATOR_H__
#define __BTREE_ITERATOR_H__

#define MAX_TERM_LENGTH 1024

class ANT_search_engine;

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
	~ANT_btree_iterator();

	char *first(char *term);
	char *next(void);
} ;



#endif __BTREE_ITERATOR_H__
