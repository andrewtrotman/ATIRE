/*
	QUERY_BOOLEAN.H
	---------------
*/
#ifndef QUERY_BOOLEAN_H_
#define QUERY_BOOLEAN_H_

#include "string_pair.h"
#include "query_parse_tree.h"

/*
	class ANT_QUERY_BOOLEAN
	-----------------------
*/
class ANT_query_boolean
{
private:
	static const long MAX_DEPTH = 1024;		 // maximum number of sets of nested brackets

public:
	enum { ERROR_NONE, ERROR_NESTED_TOO_DEEP, ERROR_PREMATURE_END_OF_QUERY, ERROR_MISSING_RIGHT_IN_SUBEXPRESSION } ;

private:
	long error_code;					// parse error code (TRUE or FALSE)
	char *query, *next_character;		// the query and where we are in it.
	ANT_string_pair token_peek;			// one token look-ahead (the lookahead token)
	long default_operator;				// default operator between query terms in a boolean query with multiple terms and no boolean operator between them

private:
	ANT_query_parse_tree *new_node(void) { return new ANT_query_parse_tree; }

protected:
	ANT_query_parse_tree *parse(long depth = 0);
	ANT_string_pair *make_token(ANT_string_pair *token);
	ANT_string_pair *peek_token(void);
	ANT_string_pair *get_token(ANT_string_pair *next_token);
	
public:
	ANT_query_boolean(void) {}
	virtual ~ANT_query_boolean() {}

	ANT_query_parse_tree *parse(char *query);
	long get_error(void) { return error_code; }
} ;

#endif /* QUERY_BOOLEAN_H_ */
