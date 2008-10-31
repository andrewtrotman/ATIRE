/*
	BTREE_HEAD_NODE.H
	-----------------
*/

#ifndef __BTREE_HEAD_NODE_H__
#define __BTREE_HEAD_NODE_H__

class ANT_memory_index_hash_node;

class ANT_btree_head_node
{
public:
	ANT_memory_index_hash_node *node;
	long long disk_pos;
} ;

#endif __BTREE_HEAD_NODE_H__
