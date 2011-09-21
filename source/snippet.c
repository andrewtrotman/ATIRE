/*
	SNIPPET.C
	---------
*/
#include <stdio.h>
#include "snippet.h"
#include "parser_token.h"
#include "parser.h"
#include "NEXI_term_ant.h"
#include "NEXI_term_iterator.h"
#include "NEXI_ant.h"

/*
	ANT_SNIPPET::ANT_SNIPPET()
	--------------------------
*/
ANT_snippet::ANT_snippet(unsigned long max_length, long length_of_longest_document)
{
keyword_hit = new ANT_snippet_keyword[(length_of_longest_document + 1) / 2 + 1]; // worst case is that every second character is a word (+1 for NULL termination)
parser = new ANT_parser();
maximum_snippet_length = max_length;
this->length_of_longest_document = length_of_longest_document;
}

/*
	ANT_SNIPPET::~ANT_SNIPPET()
	---------------------------
*/
ANT_snippet::~ANT_snippet()
{
delete [] keyword_hit;
delete parser;
}

/*
	ANT_SNIPPET::CMP_TERM()
	-----------------------
*/
int ANT_snippet::cmp_term(const void *a, const void *b)
{
ANT_parser_token *one = (ANT_parser_token *)a;
ANT_NEXI_term_ant **two = (ANT_NEXI_term_ant **)b;

return one->normalized.true_strcmp(&((*two)->term));
}

/*
	ANT_SNIPPET::CMP_CHAR_TERM()
	----------------------------
*/
int ANT_snippet::cmp_char_term(const void *a, const void *b)
{
char *one = (char *)a;
ANT_NEXI_term_ant **two = (ANT_NEXI_term_ant **)b;

return - (*two)->term.true_strcmp(one);
}


/*
	ANT_SNIPPET::GENERATE_TERM_LIST()
	---------------------------------
*/
ANT_NEXI_term_ant **ANT_snippet::generate_term_list(char *query, long *terms_in_query_out, ANT_stem *stemmer)
{
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
	Bung them into a NULL terminated array
*/
term_list = new ANT_NEXI_term_ant *[terms_in_query];
current_term = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	if (term_string->term.string() != NULL)
		term_list[current_term++] = term_string;
term_list[current_term++] = NULL;

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

/*
	ANT_SNIPPET::NEXT_N_CHARACTERS_AFTER()
	--------------------------------------
*/
char *ANT_snippet::next_n_characters_after(char *snippet, long maximum_snippet_length, char *starting_point)
{
ANT_parser_token *token;
size_t substring_length, length_in_bytes;
char *into, *start;

/*
	Initialise
*/
into = snippet;
substring_length = 0;
start = NULL;
length_in_bytes = 0;

/*
	Parse looking for non XML stuff
*/
while ((token = parser->get_next_token()) != NULL)
	if (token->string() >= starting_point)
		{
		if (token->type == TT_TAG_OPEN || token->type == TT_TAG_CLOSE)
			{
			/*
				Cut out XML tags by copying the remaining content
			*/
			if (start != NULL)
				{
				strncpy(into, start, substring_length);
				into += substring_length;
				*into++ = ' ';
				length_in_bytes += substring_length + 1;			// +1 to include the space
				}
			substring_length = 0;
			start = NULL;
			}
		else if (token->type == TT_WORD || token->type == TT_NUMBER)
			{
			/*
				Include text and numbers
			*/
			if (start == NULL)
				start = token->string();

			if (length_in_bytes + (token->string() + token->length() - start) >= maximum_snippet_length)
				break;

			substring_length = token->string() + token->length() - start;
			}
		}

/*
	Tack the final content on the end
*/
if (start != NULL)
	{
	strncpy(into, start, substring_length);
	into += substring_length;
	}

*into = '\0';

return snippet;
}