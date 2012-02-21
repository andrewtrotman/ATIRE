/*
	QUERY_BOOLEAN.H
	---------------
*/
#ifndef QUERY_BOOLEAN_H_
#define QUERY_BOOLEAN_H_

#include "string_pair.h"
#include "query_parse_tree.h"

class ANT_query;
class ANT_thesaurus;

/*
	class ANT_QUERY_BOOLEAN
	-----------------------
*/
class ANT_query_boolean
{
private:
	static const long MAX_DEPTH = 1024;		// maximum number of sets of nested brackets
	static const long MAX_NODES = 1024;		// maximum number of nodes in the parse tree

private:
	long leaves;						// number of leaf nodes (search terms) in the query
	long error_code;					// parse error code (TRUE or FALSE)
	char *query, *next_character;		// the query and where we are in it.
	ANT_string_pair token_peek;			// one token look-ahead (the lookahead token)
	long default_operator;				// default operator between query terms in a boolean query with multiple terms and no boolean operator between them
	long query_is_disjunctive;			// if the query is completely disjunctive then we can use the top-k impact ordered pruned search engine

	ANT_query_parse_tree *node_list;	// controled memory management
	long nodes_used;					// How many of the node_list are used
	ANT_thesaurus *expander;			// this is used for query expansion (the words are ORd together)
	ANT_string_pair *fake_or;			// the word "OR", used in query expansion

private:
	ANT_query_parse_tree *new_node(void) { return nodes_used < MAX_NODES ? node_list + nodes_used++ : NULL; }

protected:
	ANT_query_parse_tree *parse(long depth = 0);
	ANT_string_pair *make_token(ANT_string_pair *token);
	ANT_string_pair *peek_token(void);
	ANT_string_pair *get_token(ANT_string_pair *next_token);
	ANT_query_parse_tree *make_leaf(char *term);
	ANT_query_parse_tree *make_leaf(ANT_string_pair *term);
	ANT_query_parse_tree *make_extended_leaf(ANT_string_pair *term);
	
public:
	ANT_query_boolean(void) { node_list = new ANT_query_parse_tree[MAX_NODES]; fake_or = new ANT_string_pair("OR"); }
	virtual ~ANT_query_boolean() { delete [] node_list; delete fake_or; }

	ANT_query *parse(ANT_query *into, char *query, long default_operator = ANT_query_parse_tree::BOOLEAN_OR);
	long get_error(void) { return error_code; }
	void set_thesaurus(ANT_thesaurus *expander) { this->expander = expander; }
} ;

#endif /* QUERY_BOOLEAN_H_ */
