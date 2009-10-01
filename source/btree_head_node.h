/*
	BTREE_HEAD_NODE.H
	-----------------
*/

#ifndef BTREE_HEAD_NODE_H_
#define BTREE_HEAD_NODE_H_

class ANT_memory_index_hash_node;

class ANT_btree_head_node
{
public:
	ANT_memory_index_hash_node *node;
	long long disk_pos;
} ;

#endif  /* BTREE_HEAD_NODE_H_ */
