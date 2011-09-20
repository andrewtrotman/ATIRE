/*
	SNIPPET_TF.C
	------------
*/
#include "ctypes.h"
#include "parser.h"
#include "snippet_tf.h"

/*
	ANT_SNIPPET_TF::ANT_SNIPPET_TF()
	--------------------------------
*/
ANT_snippet_tf::ANT_snippet_tf(unsigned long max_length, long length_of_longest_document) : ANT_snippet(max_length, length_of_longest_document)
{
this->length_of_longest_document = length_of_longest_document;
}

/*
	ANT_SNIPPET_TF::GET_SNIPPET()
	-----------------------------
*/
char *ANT_snippet_tf::get_snippet(char *snippet, char *document, char *query)
{
long query_length, found;
double best_score, score;
ANT_NEXI_term_ant **term_list;
ANT_parser_token *token;
ANT_snippet_keyword *window_start, *window_end, *current, *window;
size_t padding;

/*
	get a list of all the search terms out of the query
*/
term_list = generate_term_list(query, &query_length);

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	Parse the document looking for occurences of search terms
	bung those into a NULL terminated array so that we can count them later
*/
found = 0;
while ((token = parser->get_next_token()) != NULL)
	if (token->type == TT_WORD || token->type == TT_NUMBER)
		if (bsearch(token, term_list, query_length, sizeof(*term_list), cmp_term) != NULL)
			{
			keyword_hit[found].keyword = *token;
			keyword_hit[found].score = 1;
			found++;
			}
keyword_hit[found].score = 0;

/*
	Find the start of the window that contains the most occurrences of the search terms
*/
best_score = 0;
window_start = window_end = keyword_hit;
for (current = keyword_hit; current->score != 0; current++)
	{
	score = 0;
	for (window = current; window->score != 0 && (window->keyword.string() + window->keyword.length()) - current->keyword.string() < maximum_snippet_length; window++)
		score += window->score;

	if (score > best_score)
		{
		window_start = current;
		window_end = window == current ? current : window - 1;		// the previous one (if there was one)
		best_score = score;
		}
	}

/*
	how much content should be placed at the beginning of the snippet in order to center on the center of the keywords
*/
padding = (maximum_snippet_length - (window_end->keyword.string() - window_start->keyword.string())) / 2;

/*
	Now generate the snippet and clean it up
*/
parser->set_document(document);
next_n_characters_after(snippet, maximum_snippet_length, window_start->keyword.string() - padding);
strip_duplicate_space_inline(snippet);

/*
	Return it
*/
return snippet;
}
