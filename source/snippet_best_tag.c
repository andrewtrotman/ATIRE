/*
	SNIPPET_BEST_TAG.C
	------------------
*/
#include "ctypes.h"
#include "parser.h"
#include "snippet_best_tag.h"
#include "search_engine.h"
#include "stem.h"

/*
	ANT_SNIPPET_BEST_TAG::ANT_SNIPPET_BEST_TAG()
	--------------------------------------------
*/
ANT_snippet_best_tag::ANT_snippet_best_tag(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, char *tag) : ANT_snippet(max_length, length_of_longest_document, engine, stemmer)
{
this->tag = strnew(tag);
tag_length = strlen(tag);
}

/*
	ANT_SNIPPET_BEST_TAG::GET_SNIPPET()
	-----------------------------------
	The snippet is the best <tag> element with respect to the score.  The snippet starts immediately after the start <tag>
	The score is either the tf.icf score or the term count depending on which class is being used.
*/
char *ANT_snippet_best_tag::get_snippet(char *snippet, char *document)
{
ANT_parser_token *token;
ANT_NEXI_term_ant **current_keyword;
double best_score, score;
long in_tag, hit;
char *start, *best_start;

/*
	set the term weights
*/
for (current_keyword = term_list; *current_keyword != NULL; current_keyword++)
	if (engine == NULL)
		(*current_keyword)->tf_weight = (*current_keyword)->rsv_weight = 1;
	else
		(*current_keyword)->tf_weight = (*current_keyword)->rsv_weight = log(engine->get_collection_length() / (*current_keyword)->tf_weight);

/*
	Initialise the parser
*/
parser->set_document(document);

/*
	Parse the document looking for occurences of search terms
	bung those into a NULL terminated array so that we can count them later
*/
best_score = score = 0;
best_start = start = document;
in_tag = false;
while ((token = parser->get_next_token()) != NULL)
	if (!in_tag)
		{
		if (token->type == TT_TAG_OPEN && tag_length == token->length() && strnicmp(token->string(), tag, tag_length) == 0)
			{
			start = strchr(token->string() + token->length(), '>');
			score = 0;
			in_tag = true;
			}
		}
	else
		{
		if (token->type == TT_TAG_CLOSE)
			{
			if (tag_length == token->length() - 1 && strnicmp(token->string() + 1, tag, tag_length) == 0)
				{
				in_tag = false;
				if (score > best_score)
					{
					best_score = score;
					best_start = start;
					}
				}
			}
		else if (token->type == TT_WORD || token->type == TT_NUMBER)
			{
			hit = false;
			if (stemmer == NULL || token->type == TT_NUMBER)
				{
				if ((current_keyword = (ANT_NEXI_term_ant **)bsearch(token, term_list, terms_in_query, sizeof(*term_list), cmp_term)) != NULL)
					hit = true;
				}
			else
				{
				token->normalized_pair()->strncpy(unstemmed_term, MAX_TERM_LENGTH);
				stemmer->stem(unstemmed_term, stemmed_term);
				if ((current_keyword = (ANT_NEXI_term_ant **)bsearch(stemmed_term, term_list, terms_in_query, sizeof(*term_list), cmp_char_term)) != NULL)
					hit = true;
				}

			if (hit)
				score += (*current_keyword)->tf_weight;
			}
		}

//printf("\nSNIPPET BEST SCORE:%f\n", best_score);

/*
	Now generate the snippet and clean it up
*/
XML_to_text(document_text, best_start);
parser->set_document(document_text);
next_n_characters_after(snippet, maximum_snippet_length);
strip_duplicate_space_inline(snippet);

/*
	Return it
*/
return snippet;
}
