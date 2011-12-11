/*
	SEARCH_ENGINE_BTREE_NODE.H
	--------------------------
*/

#ifndef SEARCH_ENGINE_BTREE_NODE_H_
#define SEARCH_ENGINE_BTREE_NODE_H_

#include "btree.h"

/*
	class ANT_SEARCH_ENGINE_BTREE_NODE
	----------------------------------
*/
class ANT_search_engine_btree_node
{
public:
	char *term;
	long term_length;		// strlen(term)
	long long disk_pos;
} ;

#endif  /* SEARCH_ENGINE_BTREE_NODE_H_ */
