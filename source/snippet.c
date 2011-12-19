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
#include "stem.h"
#include "search_engine.h"

/*
	ANT_SNIPPET::ANT_SNIPPET()
	--------------------------
*/
ANT_snippet::ANT_snippet(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer)
{
keyword_hit = new ANT_snippet_keyword[(length_of_longest_document + 1) / 2 + 1]; // worst case is that every second character is a word (+1 for NULL termination)
term_list = NULL;
parser = new ANT_parser();
maximum_snippet_length = max_length;
this->length_of_longest_document = length_of_longest_document;

this->engine = engine;
if ((this->stemmer = engine == NULL ? NULL : engine->get_stemmer()) == NULL)
	this->stemmer = stemmer;

*query_buffer = *unstemmed_term = *stemmed_term = '\0';

document_text = new char [length_of_longest_document + 1];
}

/*
	ANT_SNIPPET::~ANT_SNIPPET()
	---------------------------
*/
ANT_snippet::~ANT_snippet()
{
delete [] term_list;
delete [] keyword_hit;
delete parser;
delete [] document_text;
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
	ANT_SNIPPET::PARSE_QUERY()
	--------------------------
*/
ANT_NEXI_term_ant **ANT_snippet::parse_query(char *query)
{
long current_term;
ANT_NEXI_term_ant *parse_tree, *term_string;
ANT_NEXI_term_iterator term;
char *into = query_buffer;
size_t stem_length;
ANT_search_engine_btree_leaf details;
ANT_search_engine_btree_leaf *leaf;

/*
	Initialise the parser with the query string
*/
parse_tree = NEXI_parser.parse(query);		// needs fixing for boolean queries

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
if (term_list != NULL)
	delete [] term_list;
term_list = new ANT_NEXI_term_ant *[terms_in_query + 1];		// +1 because the list is NULL terminated

current_term = 0;
for (term_string = (ANT_NEXI_term_ant *)term.first(parse_tree); term_string != NULL; term_string = (ANT_NEXI_term_ant *)term.next())
	if (term_string->term.string() != NULL)
		{
		/*
			Compute the CF value
		*/
		strncpy(unstemmed_term, term_string->term.string(), term_string->term.length());
		unstemmed_term[term_string->term.length()] = '\0';
		if (engine == NULL)
			term_string->tf_weight = 1;
		else
			{
			leaf = engine->get_collection_frequency(unstemmed_term, stemmer, &details);
			if (leaf != NULL)
				term_string->tf_weight = (double)leaf->global_collection_frequency;
			else
				term_string->tf_weight = 0;			// the query term is not in the collection
			}

		/*
			Now compute the term itself (i.e. the stem if there is one)
		*/
		if (stemmer != NULL)
			{
			stemmer->stem(unstemmed_term, stemmed_term);
			stem_length = strlen(stemmed_term);
			if (into + stem_length >= query_buffer + sizeof(query_buffer))
				break;
			else
				{
				/*
					Now put the stemmed term into the list
				*/
				strcpy(into, stemmed_term);
				term_string->term.start = into;
				term_string->term.string_length = stem_length;
				into += stem_length + 1;
				}
			}
		term_list[current_term++] = term_string;
		}
term_list[current_term++] = NULL;

/*
	sort
*/
qsort(term_list, terms_in_query, sizeof(*term_list), ANT_NEXI_term_ant::cmp_term);

/*
	return
*/
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
char *into, *start, *end_of_snippet, *from;

/*
	Initialise
*/
end_of_snippet = snippet + maximum_snippet_length - 1;
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
				length_in_bytes += substring_length;

				/*
					Append any punctuation that is after the last term but before the open / close tag
				*/
				for (from = start + substring_length; *from != '<'; from++)
					if (into < end_of_snippet)
						{
						*into++ = *from;
						length_in_bytes++;
						}
				/*
					Put a space between tags even if there isn't one in the text
				*/
				if (into < end_of_snippet)
					{
					*into++ = ' ';
					length_in_bytes++;
					}
				}
			substring_length = 0;
			start = NULL;
			}
		else
			{
			/*
				in the case of punctuation we might be at the end of a tag so skip over it looking for true punctuation
			*/
			if (token->type == TT_PUNCTUATION)
				while (token->string_length > 0 && (token->start[0] == '<' || token->start[0] == '>'))
					{
					token->start++;
					token->string_length--;
					}
			/*
				if we've got some punctuation of some text of a number then include it
			*/
			if (token->length() > 0)
				{
				/*
					Include text and numbers
				*/
				if (start == NULL)
					start = token->string();

				if (length_in_bytes + (token->string() + token->length() - start) >= (size_t)maximum_snippet_length)
					break;

				substring_length = token->string() + token->length() - start;
				}
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


/*
	ANT_SNIPPET::XML_TO_TEXT()
	--------------------------
	By destination need be no longer than source as worst case is no change
*/
char *ANT_snippet::XML_to_text(char *destination, char *source)
{
char *from, *into;
long in_tag;

from = source;
into = destination;
in_tag = false;
for (from = source; *from != '\0'; from++)
	{
	if (!in_tag && *from == '<')
		in_tag = true;

	if (!in_tag)
		*into++ = *from;

	if (in_tag && *from == '>')
		in_tag = false;
	}

*into = '\0';
return destination;
}