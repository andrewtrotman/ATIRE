/*
	FOCUS.C
	-------
*/
#include <string.h>
#include "hash_table.h"	
#include "string_pair.h"
#include "focus.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_FOCUS::ANT_FOCUS()
	----------------------
*/
ANT_focus::ANT_focus(ANT_focus_results_list *factory)
{
/*
	Empty the hash table and initialise the current_node pointer
*/
memset(hash_table, 0, sizeof(hash_table));
nodes_allocated = 0;
this->result_factory = factory;
}

/*
	ANT_FOCUS::NEW_HASH_NODE()
	--------------------------
*/
ANT_focus_hash_node *ANT_focus::new_hash_node(ANT_string_pair *string)
{
ANT_focus_hash_node *current;

/*
	Not enough room to fit another key word so fail (unlikely to exceed HASH_NODES search terms)
*/
if (nodes_allocated >= HASH_NODES)
	return NULL;
/*
	Initialise the new node
*/
current = node_store + nodes_allocated;
current->left = current->right = NULL;
current->string = string;

/*
	Node to use next time
*/
nodes_allocated++;

/*
	Return current node
*/
return current;
}

/*
	ANT_FOCUS::FIND_NODE()
	----------------------
*/
ANT_focus_hash_node *ANT_focus::find_node(ANT_focus_hash_node *root, ANT_string_pair *string)
{
long cmp;

/*
	Simple binary search for the given string
*/
while ((cmp = string->strcmp(root->string)) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			return NULL;
		else
			root = root->left;
	else
		if (root->right == NULL)
			return NULL;
		else
			root = root->right;
	}
return root;
}

/*
	ANT_FOCUS::FIND_ADD_NODE()
	--------------------------
*/
ANT_focus_hash_node *ANT_focus::find_add_node(ANT_focus_hash_node *root, ANT_string_pair *string)
{
long cmp;

/*
	Simple binary search for the given string and if we fail then insert into the tree
	note that there is no effort to ballance the tree as terms will probably be presented
	in idf order which is (probably) close to random.  There's also no point in ballancin
	a tiny tree.
*/
while ((cmp = string->strcmp(root->string)) != 0)
	{
	if (cmp > 0)
		if (root->left == NULL)
			return root->left = new_hash_node(string);
		else
			root = root->left;
	else
		if (root->right == NULL)
			return root->right = new_hash_node(string);
		else
			root = root->right;
	}

return root;
}

/*
	ANT_FOCUS::MATCH()
	------------------
*/
long ANT_focus::match(ANT_string_pair *string)
{
long hash = ANT_hash_8(string);

if (hash_table[hash] == 0)
	return FALSE;
else
	return find_node(hash_table[hash], string) == NULL ? FALSE : TRUE;
}

/*
	ANT_FOCUS::ADD_TERM()
	---------------------
*/
long ANT_focus::add_term(ANT_string_pair *string)
{
ANT_focus_hash_node *answer;
long hash = ANT_hash_8(string);

if (hash_table[hash] == 0)
	answer = hash_table[hash] = new_hash_node(string);
else
	answer = find_add_node(hash_table[hash], string);

return answer == NULL ? FALSE : TRUE;
}

