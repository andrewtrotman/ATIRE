/*
	SNIPPET.C
	---------
*/
#include "snippet.h"
#include "parser_token.h"
#include "NEXI_term_ant.h"
#include "NEXI_term_iterator.h"
#include "NEXI_ant.h"

/*
	ANT_SNIPPET::CMP_TERM()
	-----------------------
*/
int ANT_snippet::cmp_term(const void *a, const void *b)
{
ANT_parser_token *one = (ANT_parser_token *)a;
ANT_NEXI_term_ant **two = (ANT_NEXI_term_ant **)b;

return one->true_strcmp(&((*two)->term));
}

/*
	ANT_SNIPPET::GENERATE_TERM_LIST()
	---------------------------------
*/
ANT_NEXI_term_ant **ANT_snippet::generate_term_list(char *query, long *terms_in_query_out)
{
ANT_NEXI_ant NEXI_parser;
long terms_in_query, current_term;
ANT_NEXI_term_ant *parse_tree, *term_string, **term_list;
ANT_NEXI_term_iterator term;

parse_tree = NEXI_parser.parse(query);

/*
	Count the number of terms in the query
*/
terms_in_query = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	if (term_string->term.string() != NULL)
		terms_in_query++;

/*
	Bung them into an array
*/
term_list = new ANT_NEXI_term_ant *[terms_in_query];
current_term = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	if (term_string->term.string() != NULL)
		term_list[current_term++] = term_string;

/*
	sort
*/

qsort(term_list, terms_in_query, sizeof(*term_list), ANT_NEXI_term_ant::cmp_term);

/*
	return
*/
*terms_in_query_out = terms_in_query;
return term_list;
}
