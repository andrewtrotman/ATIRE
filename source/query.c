/*
	QUERY.C
	-------
*/
#include <stdio.h>
#include "query.h"

/*
	ANT_QUERY::CLEAR()
	------------------
*/
void ANT_query::clear(void)
{
feedback_terms = NULL;
type = BLANK;
subtype = DISJUNCTIVE;
NEXI_query = NULL;
boolean_query = NULL;
parse_error = ERROR_NONE;
terms_in_query = feedback_terms_in_query = 0;
}

/*
	ANT_QUERY::SET_QUERY()
	----------------------
*/
void ANT_query::set_query(ANT_NEXI_term_ant *query)
{
clear();
type = NEXI;
subtype = DISJUNCTIVE;
NEXI_query = query;
}

/*
	ANT_QUERY::SET_QUERY()
	----------------------
*/
void ANT_query::set_query(ANT_query_parse_tree *query)
{
clear();
type = BOOLEAN;
boolean_query = query;
}
