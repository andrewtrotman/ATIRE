/*
	NEXI_ANT.C
	----------
*/
#include "NEXI_ant.h"
#include "query.h"

/*
	ANT_NEXI_ANT::PARSE()
	---------------------
*/
ANT_query *ANT_NEXI_ant::parse(ANT_query *into, char *expression)
{
ANT_string_pair terms;
ANT_NEXI_term *answer;

error_code = ANT_query::ERROR_NONE;
at = string = (unsigned char *)expression;

get_next_token();

if (token.true_strcmp("//") == 0)		// we're a CAS query
	answer = read_CAS();
else
	answer = read_CO(NULL, &terms);

if (token[0] != '\0')
	parse_error(ANT_query::ERROR_PREMATURE_END_OF_QUERY, "Unexpected end of query");

into->clear();
into->set_query((ANT_NEXI_term_ant *)answer);
into->set_error(error_code);

return into;
}