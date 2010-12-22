/*
	QUERY.H
	-------
*/
#ifndef QUERY_H_
#define QUERY_H_

class ANT_NEXI_term_ant;
class ANT_query_boolean;
class ANT_query_parse_tree;
class ANT_memory_index_one_node;

/*
	class ANT_QUERY
	---------------
*/
class ANT_query
{
public:
	enum { 
		ERROR_NONE, 
		ERROR_NO_MEMORY, 
		ERROR_NESTED_TOO_DEEP, 
		ERROR_PREMATURE_END_OF_QUERY, 
		ERROR_MISSING_RIGHT_IN_SUBEXPRESSION, 
		ERROR_MISSING_OPEN_ROUND_BRACKET, 
		ERROR_MISSING_CLOSE_ROUND_BRACKET, 
		ERROR_MISSING_OPEN_SQUARE_BRACKET, 
		ERROR_MISSING_CLOSE_SQUARE_BRACKET, 
		ERROR_MISSING_COMMA,
		ERROR_MISSING_DOUBLE_SLASH,
		ERROR_MISSING_TERM,
		ERROR_MISSING_ABOUT_CLAUSE,
		ERROR_INVALID_PHRASE
		} ;

	enum { BLANK, NEXI, BOOLEAN };		// query parse type
	enum { DISJUNCTIVE, CONJUNCTIVE };	// query sub-type

public:
	long type;							// NEXI or BOOLEAN
	long subtype;						// DISJUNCTIVE or CONJUNCTIVE

	ANT_NEXI_term_ant *NEXI_query;
	ANT_query_parse_tree *boolean_query;
	ANT_memory_index_one_node **feedback_terms;
	long parse_error;
	long terms_in_query;				// leaf nodes (search terms) 0 = unknown
	long feedback_terms_in_query;

public:
	ANT_query() { clear(); }
	virtual ~ANT_query() {}

	void set_query(ANT_NEXI_term_ant *query);
	void set_query(ANT_query_parse_tree *query);

	void set_subtype(long to) { subtype = to; }
	void set_error(long code) { parse_error = code; }

	void clear(void);
} ;

#endif /* QUERY_H_ */
