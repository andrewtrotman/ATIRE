/*
	QUERY_PARSE_TREE.H
	------------------
*/
#ifndef QUERY_PARSE_TREE_H_
#define QUERY_PARSE_TREE_H_

#include "string_pair.h"

/*
	class ANT_QUERY_PARSE_TREE
	--------------------------
*/
class ANT_query_parse_tree
{
public:
	enum { LEAF_NODE, BOOLEAN_AND, BOOLEAN_OR, BOOLEAN_NOT, BOOLEAN_XOR };

public:
	long boolean_operator;				// one of the enum above
	ANT_string_pair term;				// if a leaf node
	ANT_query_parse_tree *left, *right;	// if an internal node

private:
	void internal_text_render(void);

public:
	void text_render(void);
} ;

#endif /* QUERY_PARSE_TREE_H_ */

