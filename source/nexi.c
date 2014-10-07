/*
	NEXI.C
	------
*/
#include "NEXI.h"
#include "ctypes.h"
#include "parser.h"
#include "NEXI_term.h"
#include "NEXI_term_ant.h"
#include "query.h"
#include "thesaurus.h"
#include "thesaurus_relationship.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_NEXI::GET_NEXI_TERM()
	-------------------------
*/
ANT_NEXI_term *ANT_NEXI::get_NEXI_term(ANT_NEXI_term *parent, ANT_string_pair *tag, char *term, long weight)
{
ANT_string_pair pair(term);

return get_NEXI_term(parent, tag, &pair, weight);
}

/*
	ANT_NEXI::GET_NEXI_TERM()
	-------------------------
*/
ANT_NEXI_term *ANT_NEXI::get_NEXI_term(ANT_NEXI_term *parent, ANT_string_pair *tag, ANT_string_pair *term, long weight)
{
ANT_NEXI_term *answer;

answer = next_free_node();

pool_used++;
if (pool_used >= MAX_NEXI_TERMS)
	{
	parse_error(ANT_query::ERROR_PREMATURE_END_OF_QUERY, "Out of ANT_NEXI_term nodes while parsing the query");
	pool_used = MAX_NEXI_TERMS - 1;
	}

answer->next = answer->parent_path = NULL;
answer->sign = weight;
answer->rsv_weight = answer->tf_weight = answer->query_frequency = 1;

if (tag == NULL)
	{
	answer->path.start = NULL;
	answer->path.string_length = 0;
	}
else
	answer->path = *tag;

if (term == NULL)
	{
	answer->term.start = NULL;
	answer->term.string_length = 0;
	}
else
	answer->term = *term;

if (parent != NULL)
	parent->next = answer;

return answer;
}

/*
	ANT_NEXI::DUPLICATE_PATH_CHAIN()
	--------------------------------
*/
ANT_NEXI_term *ANT_NEXI::duplicate_path_chain(ANT_NEXI_term *start, ANT_NEXI_term **end_of_chain)
{
ANT_NEXI_term sentinal, *current, *tail;

tail = &sentinal;
for (current = start; current != NULL; current = current->next)
	tail = get_NEXI_term(tail,  &current->path, &current->term, current->sign);

*end_of_chain = tail;
return sentinal.next;
}

/*
	ANT_NEXI::ISPART()
	------------------
*/
long ANT_NEXI::ispart(unsigned char *from, long length, unsigned char *next)
{
unsigned long from_character, next_character;

if (length == 0)
	return TRUE;				/* all tokens must be at least one char in length */
else if (length == 1)
	if (*from == '.')
		return FALSE;
	else if (*from == '(')
		return FALSE;
	else if (*from == '|')
		return FALSE;
	else if (*from == ')')
		return FALSE;
	else if (*from == '-')
		if (unicode_chartype(utf8_to_wide(next)) == CT_NUMBER)
			return TRUE;
		else
			return FALSE;
	else if (*from == '/')
		if (*next == '/')
			return TRUE;
		else
			return FALSE;
	else
		{
		from_character = utf8_to_wide(from);
		next_character = utf8_to_wide(next);

		if (is_cjk_language(from_character))
			{
			/*
				If we want to generate one Chinese string for later segmentaton then
				we want to do the following line:

					return ANT_parser::is_cjk_language(next);					// this string will later need to be segmented

				otherwise Chinese words can be at best one character in length so return FALSE

				by default, we segment the whole Chinese string into single characters,
				if segmentation is set to false, we won't do anything. This could happen when we segment the string manually
				either in the console, the query file or segmented query passed by other programs
			*/
			if (segmentation)
				return FALSE;
			return is_cjk_language(next_character);
			}
		else if ((unicode_xml_class(from_character) & XMLCC_NAME_START) == XMLCC_NAME_START || unicode_chartype(from_character) == CT_LETTER)
			if ((unicode_xml_class(next_character) & XMLCC_NAME) == XMLCC_NAME || unicode_chartype(next_character) == CT_LETTER)
				return TRUE;
			else if (*next == '-')		// hyphenated words
				return TRUE;
			else if (*next == '\'')		// apostrophies (don't tell me)
				return TRUE;
			else
				return FALSE;
		else if (unicode_chartype(from_character) == CT_NUMBER)
			if (unicode_chartype(next_character) == CT_NUMBER)
				return TRUE;
			else
				return FALSE;
		else if (from_character == SPECIAL_TERM_CHAR)
			return ANT_isalnum(*next) || *next == ':' || *next == '-';
		else
			return FALSE;
		}
else
	{
	from_character = utf8_to_wide(from);
	next_character = utf8_to_wide(next);

	if (*from == '-')
		if (unicode_chartype(next_character) == CT_NUMBER)
			return TRUE;
		else
			return FALSE;
	else if (is_cjk_language(from_character))
		return is_cjk_language(next_character);		// This can only happen if we later segment, it can never happen without segmentation
	else if ((unicode_xml_class(from_character) & XMLCC_NAME_START) == XMLCC_NAME_START || unicode_chartype(from_character) == CT_LETTER)
		if ((unicode_xml_class(next_character) & XMLCC_NAME) == XMLCC_NAME || unicode_chartype(next_character) == CT_LETTER
				|| *next == '-' || *next == '\'')
			return TRUE;
		else
			return FALSE;
	else if (unicode_chartype(from_character) == CT_NUMBER || unicode_chartype(from_character) == CT_LETTER)
		if (unicode_chartype(next_character) == CT_NUMBER || unicode_chartype(next_character) == CT_LETTER)
			return TRUE;
		else
			return FALSE;
	else
		return FALSE;
	}
}		

/*
	ANT_NEXI::GET_NEXT_TOKEN()
	--------------------------
*/
ANT_string_pair *ANT_NEXI::get_next_token(void)
{
long bytes;
long length_in_chars, length_in_bytes;

length_in_chars = length_in_bytes = 0;

if (*at != '\0')
	{
#ifdef NEVER
	/*
		This code is very very wrong.  The parser can only skip over spaces, it cannot skip over
		the special characters that are part of the NEXI query language.  For example, "//section[about(.,NEXI)]"
		is valid NEXI and must be parsed as such.  Below is the corected version.
	*/
	/*
		Using the multi-language parser, skip over all characters that are not alphabetic or numeric.
		Also skip the non-character, e.g. symbol and punctuation in other languages
	*/
	while (*at != '\0' && unicode_chartype(utf8_to_wide(at)) != CT_NUMBER && unicode_chartype(utf8_to_wide(at)) != CT_LETTER && utf8_to_wide(at) != SPECIAL_TERM_CHAR)
		at += utf8_bytes(at);
#else
	do
		{
		if (*at == '\0')
			break;
		else if (unicode_chartype(utf8_to_wide(at)) == CT_NUMBER)
			break;
		else if (unicode_chartype(utf8_to_wide(at)) == CT_LETTER)
			break;
		else if (utf8_to_wide(at) == SPECIAL_TERM_CHAR)
			break;
		else
			{
			if (is_cjk_language(at))
				break;
			bytes = utf8_bytes(at);
			if (bytes == 1 && !ANT_isspace(*at))
				break;

			at += bytes;
			}
		}
	while (*at != '\0');
#endif

	if (*at != '\0')
		while (*(at + length_in_bytes) != '\0' && ispart(at, length_in_chars, at + length_in_bytes))
			{
			length_in_chars++;
			length_in_bytes += utf8_bytes(at + length_in_bytes);
			}
	}

token.start = (char *)at;
token.string_length = length_in_bytes;
at += length_in_bytes;
//printf("token.string_length = %i",token.start,token.string_length);
return &token;
}

/*
	ANT_NEXI::READ_PATH()
	---------------------
*/
void ANT_NEXI::read_path(ANT_string_pair *path)
{
long more;

path->start = token.start;
more = TRUE;
do
	{
	get_next_token();
	if (ANT_isXMLnamestartchar(token[0]))
		continue;
	else if (token.true_strcmp("//") == 0)
		continue;
	else if (token[0] == 0)
		more = FALSE;
	else if (strchr("(|)*@", token[0]))
		continue;
	else
		more = FALSE;
	}
while (more);

/*
	Convert the whole path into a single 
*/
path->string_length = token.start - path->start;
}

/*
	ANT_NEXI::PARSE_ERROR()
	-----------------------
*/
void ANT_NEXI::parse_error(long code, char *message)
{
if (noisy_errors)
	{
	if (first_error_pos < 0)
		first_error_pos = (long)(at - string);
	fprintf(stderr, "NEXI ERROR, column %ld: %s\n", (long)(at - string), message);
	}
error_code = code;
}

/*
	ANT_NEXI::ABOUT()
	-----------------
*/
ANT_NEXI_term *ANT_NEXI::about(void)
{
ANT_NEXI_term *answer;
ANT_string_pair path, terms;

get_next_token();
if (token[0] != '(')
	parse_error(ANT_query::ERROR_MISSING_OPEN_ROUND_BRACKET, "Expected '('");
get_next_token();						// prime the read_path method with the first token in the path
read_path(&path);
if (token[0] != ',')
	parse_error(ANT_query::ERROR_MISSING_COMMA, "Expected ','");
get_next_token();			// prime read_CO
answer = read_CO(&path, &terms);
if (token[0] != ')')
	parse_error(ANT_query::ERROR_MISSING_CLOSE_ROUND_BRACKET, "Expected ')'");

return answer;
}

/*
	ANT_NEXI::READ_OPERATOR()
	-------------------------
*/
void ANT_NEXI::read_operator(void)
{
while (strchr("<>=", token[0]) != NULL)
	if (token[0] == '\0')
		break;
	else
		get_next_token();
}

/*
	ANT_NEXI::NUMBERS()
	-------------------
*/
ANT_NEXI_term *ANT_NEXI::numbers(void)
{
ANT_string_pair path, term;
ANT_NEXI_term *answer;

read_path(&path);
read_operator();		// at this point we should do something with the operator
term = token;

answer = get_NEXI_term(NULL, &path, &term, 0);

return answer;
}

/*
	ANT_NEXI::READ_TERM()
	---------------------
*/
long ANT_NEXI::read_term(ANT_string_pair *term)
{
unsigned long character;

*term = *get_next_token();

character = utf8_to_wide(token.start);

if ((unicode_xml_class(character) & XMLCC_NAME_START)==XMLCC_NAME_START)
	return TRUE;

ANT_UNICODE_chartype chartype = unicode_chartype(character);

if (chartype == CT_LETTER || chartype == CT_NUMBER)
	return TRUE;
if (token[0] == '-' && token.string_length > 1)
	return TRUE;
if (token[0] == '"')
	return read_phrase(term);

return FALSE;
}

/*
	ANT_NEXI::READ_PHRASELESS_TERM()
	--------------------------------
*/
long ANT_NEXI::read_phraseless_term(ANT_string_pair *term)
{
unsigned long character;

*term = *get_next_token();

character = utf8_to_wide(token.start);

if ((unicode_xml_class(character) & XMLCC_NAME_START)==XMLCC_NAME_START)
	return TRUE;

ANT_UNICODE_chartype chartype = unicode_chartype(character);

if (chartype == CT_LETTER || chartype == CT_NUMBER)
	return TRUE;

if (token[0] == '-' && token.string_length > 1)
	return TRUE;
return FALSE;
}

/*
	ANT_NEXI::READ_PHRASE()
	-----------------------
*/
long ANT_NEXI::read_phrase(ANT_string_pair *string)
{
ANT_string_pair current;
long more;

string->start = token.start;
do
	more = read_phraseless_term(&current);
while (more);

if (token[0] != '"')
	{
	parse_error(ANT_query::ERROR_INVALID_PHRASE, "non-term found in phrase");
	return FALSE;
	}

string->string_length = token.start - string->start + token.string_length;
return TRUE;
}

/*
	ANT_NEXI::READ_CO()
	-------------------
*/
ANT_NEXI_term *ANT_NEXI::read_CO(ANT_string_pair *path, ANT_string_pair *terms)
{
ANT_string_pair current;
long weight, more = TRUE;
ANT_NEXI_term root, *node;
unsigned long character;

root.next = NULL;
node = &root;		// root acts as a sentinal
current = *terms = token;
do
	{
	weight = 0;
	if (token.string_length == 1)
		{
		if (token[0] == '"')
			read_phrase(&current);
		else if (token[0] == '+')		// positive selected terms
			{
			weight = +1;
			if (!read_term(&current))
				parse_error(ANT_query::ERROR_MISSING_TERM, "term expected");
			}
		else if (token[0] == '-')		// negatively selected terms
			{
			weight = -1;
			if (!read_term(&current))
				parse_error(ANT_query::ERROR_MISSING_TERM, "term expected");
			}
		else
			{
			character = utf8_to_wide(token.start);

			if (unicode_chartype(character) != CT_LETTER && unicode_chartype(character) != CT_NUMBER)
				more = FALSE;
			}
		}
	else
		{
		character = utf8_to_wide(token.start);

		if (unicode_chartype(character) != CT_LETTER && unicode_chartype(character) != CT_NUMBER && token[0] != '-' && !is_cjk_language(character))
			more = FALSE;
		}
	if (more)
		{
		/*
			At this point we have a search term and so we can do a thesaurus expansion (or (in the future) translate into another language)
		*/
		node = get_NEXI_term(node, path, &current, weight);
		if (expander != NULL)
			{
			ANT_thesaurus_relationship *synset;
			long long size_of_synset, alternate_term;

			synset = expander->get_synset(&current, &size_of_synset);
			for (alternate_term = 0; alternate_term < size_of_synset; alternate_term++)
				node = get_NEXI_term(node, path, synset[alternate_term].term, weight);
			}
		current = *get_next_token();
		}
	}
while (more);

terms->string_length = token.start - terms->start;

return root.next;
}

/*
	ANT_NEXI::READ_CAS()
	--------------------
*/
ANT_NEXI_term *ANT_NEXI::read_CAS(void)
{
ANT_string_pair path;
ANT_NEXI_term *got, sentinal, *current, *parent_path, *end_of_path_chain;

current = &sentinal;
end_of_path_chain = sentinal.next = parent_path = NULL;
do
	{
	read_path(&path);
	if (path.string_length < 3 || strncmp(path.start, "//", 2) != 0)
		parse_error(ANT_query::ERROR_MISSING_DOUBLE_SLASH, "Path must start with a //");
	if (parent_path != NULL)
		{
		parent_path = duplicate_path_chain(parent_path, &end_of_path_chain);
		get_NEXI_term(end_of_path_chain, &path);
		}
	else
		parent_path = get_NEXI_term(end_of_path_chain, &path);

	if (token[0] == '[')
		{
		do
			{
			get_next_token();

			got = NULL;
			if (token.true_strcmp("about") == 0)
				got = about();
			else if (token[0] == '.')
				got = numbers();
			else
				parse_error(ANT_query::ERROR_MISSING_ABOUT_CLAUSE, "Expected 'about()' clause or numeric expression");

			if (got != NULL)
				{
				current->next = got;
				while (current->next != NULL)
					{
					current = current->next;
					current->parent_path = parent_path;
					}
				}
			
			get_next_token();
			}
		while (token.true_strcmp("or") == 0 || token.true_strcmp("and") == 0 || token.true_strcmp("OR") == 0 || token.true_strcmp("AND") == 0);
		if (token[0] != ']')
			parse_error(ANT_query::ERROR_MISSING_CLOSE_SQUARE_BRACKET, "Expected ']'");
		get_next_token();		// primed for next iteration of target-element loop
		}
	else if (token[0] != '\0')
		parse_error(ANT_query::ERROR_MISSING_OPEN_SQUARE_BRACKET, "Expected '['");
	}
while (token[0] != '\0');

return sentinal.next;
}

/*
	ANT_NEXI::PARSE()
	-----------------
*/
ANT_NEXI_term *ANT_NEXI::parse(char *expression)
{
ANT_query into;

rewind();
parse(&into, expression);

return error_code == ANT_query::ERROR_NONE ? into.NEXI_query : NULL;
}



#ifdef NEVER
	/*
		MAIN()
		------
	*/
	int main(void)
	{
	ANT_NEXI_term *parsing;
	ANT_NEXI nexi_parser;
	char buffer[1024];

	for(;;)
		{
		printf("]");
		gets(buffer);
		if (strcmp(buffer, ".quit") == 0)
			break;
		parsing = nexi_parser.parse(buffer);
		if (parsing != NULL)
			{
			if (nexi_parser.get_success_state())
				parsing->text_render();
			else
				puts("Failed to parse");
			}
		puts("");
		}

	return 0;
	}
#endif
