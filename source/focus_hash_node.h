/*
	FOCUS_HASH_NODE.H
	-----------------
*/
#ifndef FOCUS_HASH_NODE_H_
#define FOCUS_HASH_NODE_H_

class ANT_string_pair;

/*
	class ANT_FOCUS_HASH_NODE
	-------------------------
*/
class ANT_focus_hash_node
{
public:
	ANT_string_pair *string;
	ANT_focus_hash_node *left, *right;
} ;

#endif /* FOCUS_HASH_NODE_H_ */
