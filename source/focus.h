/*
	FOCUS.H
	-------
*/
#ifndef FOCUS_H_
#define FOCUS_H_

#include "focus_hash_node.h"
#include "focus_result.h"

class ANT_focus_results_list;
class ANT_search_engine_accumulator;

/*
	class ANT_FOCUS
	---------------
*/
class ANT_focus
{
private:
	static const long HASH_NODES = 1024;

private:
	ANT_focus_hash_node *hash_table[0x100];
	ANT_focus_hash_node node_store[HASH_NODES];
	long nodes_allocated;

protected:
	ANT_focus_results_list *result_factory;

private:
	ANT_focus_hash_node *find_node(ANT_focus_hash_node *root, ANT_string_pair *string);
	ANT_focus_hash_node *find_add_node(ANT_focus_hash_node *root, ANT_string_pair *string);
	ANT_focus_hash_node *new_hash_node(ANT_string_pair *string);

protected:
	long match(ANT_string_pair *string);

public:
	ANT_focus(ANT_focus_results_list *result_factory);
	virtual ~ANT_focus() {}

	long add_term(ANT_string_pair *string);
	virtual ANT_focus_result *focus(unsigned char *document, long *results_length, long long docid = 0, char *document_name = NULL, ANT_search_engine_accumulator *document_accumulator = NULL) = 0;
} ;

#endif /* FOCUS_H_ */
