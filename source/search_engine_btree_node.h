/*
	SEARCH_ENGINE_BTREE_NODE.H
	--------------------------
*/

#ifndef __SEARCH_ENGINE_BTREE_NODE_H__
#define __SEARCH_ENGINE_BTREE_NODE_H__

#include "btree.h"

class ANT_search_engine_btree_node
{
public:
	char *term;
	long long disk_pos;
} ;

#endif __SEARCH_ENGINE_BTREE_NODE_H__
