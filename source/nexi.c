/*
	NEXI.C
	------
*/
#include "NEXI.h"
#include "ctypes.h"
#include "parser.h"
#include "NEXI_term.h"

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
ANT_NEXI_term *ANT_NEXI::get_NEXI_term(ANT_NEXI_term *parent, ANT_string_pair *tag, ANT_string_pair *term, long weight)
{
ANT_NEXI_term *answer;

answer = next_free_node();

pool_used++;
if (pool_used >= MAX_NEXI_TERMS)
	{
	parse_error("Out of ANT_NEXI_term nodes while parsing the query");
	pool_used = MAX_NEXI_TERMS - 1;
	}

answer->next = answer->parent_path = NULL;
answer->sign = weight;
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
ANT_NEXI_term sentinal, *current, *head, *tail;

head = tail = &sentinal;
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
		if (ANT_isdigit(*next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_parser::isXMLnamestartchar(*from))
		if (ANT_parser::isXMLnamechar(*next))
			return TRUE;
		else if (*next == '-')		// hyphenated words
			return TRUE;
		else if (*next == '\'')		// apostrophies (don't tell me)
			return TRUE;
		else
			return FALSE;
	else if (ANT_isdigit(*from))
		if (ANT_isdigit(*next))
			return TRUE;
		else
			return FALSE;
	else if (*from == '/')
		if (*next == '/')
			return TRUE;
		else
			return FALSE;
	else if (ANT_parser::ischinese(from))
		{
		/*
			If we want to generate one Chinese string for later segmentaton then
			we want to do the following line:

				return ANT_parser::ischinese(next);					// this string will later need to be segmented

			otherwise Chinese words can be at best one character in length so return FALSE
		*/
		return FALSE;
		}
	else
		return FALSE;
else
	{
	if (*from == '-')
		if (ANT_isdigit(*next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_parser::isXMLnamestartchar(*from))
		if (ANT_parser::isXMLnamechar(*next) || *next == '-' || *next == '\'')
			return TRUE;
		else
			return FALSE;
	else if (ANT_isalnum(*from))
		if (ANT_isalnum(*next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_parser::ischinese(from))
		return ANT_parser::ischinese(next);		// This can only happen if we later segment, it can never happen without segmentation
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
long length_in_chars, length_in_bytes;

length_in_chars = length_in_bytes = 0;

if (*at != '\0')
	{
	/*
		Using the multi-language parser, skip over all characters that are not alphabetic or numberic.
	*/
	while (ANT_isspace(*at) && *at != '\0')
		at++;

	if (*at != '\0')
		while (*(at + length_in_bytes) != '\0' && ispart(at, length_in_chars, at + length_in_bytes))
			{
			length_in_chars++;
			length_in_bytes += ANT_parser::utf8_bytes(at);
			}
	}

token.start = (char *)at;
token.string_length = length_in_bytes;
at += length_in_bytes;
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
	if (ANT_parser::isXMLnamestartchar(token[0]))
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
void ANT_NEXI::parse_error(char *message)
{
fprintf(stderr, "NEXI ERROR, column %ld: %s\n", (long)(at - string), message);
successful_parse = FALSE;
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
	parse_error("Expected '('");
get_next_token();						// prime the read_path method with the first token in the path
read_path(&path);
if (token[0] != ',')
	parse_error("Expected ','");
get_next_token();			// prime read_CO
answer = read_CO(&path, &terms);
if (token[0] != ')')
	parse_error("Expected ')'");

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
*term = *get_next_token();
if (ANT_parser::isXMLnamestartchar(token[0]))
	return TRUE;
if (ANT_isalnum(token[0]))
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
*term = *get_next_token();
if (ANT_parser::isXMLnamestartchar(token[0]))
	return TRUE;
if (ANT_isalnum(token[0]))
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
	parse_error("non-term found in phrase");
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
				parse_error("term expected");
			}
		else if (token[0] == '-')		// negatively selected terms
			{
			weight = -1;
			if (!read_term(&current))
				parse_error("term expected");
			}
		else if (!ANT_isalnum(token[0]))
			more = FALSE;
		}
	else if (!ANT_isalnum(token[0]) && token[0] != '-' && !ANT_parser::ischinese(token.start))
		more = FALSE;
	if (more)
		{
		node = get_NEXI_term(node, path, &current, weight);
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
		parse_error("Path must start with a //");
	if (parent_path != NULL)
		{
		parent_path = duplicate_path_chain(parent_path, &end_of_path_chain);
		get_NEXI_term(end_of_path_chain, &path, NULL, 0);
		}
	else
		parent_path = get_NEXI_term(end_of_path_chain, &path, NULL, 0);

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
				parse_error("Expected 'about()' clause or numeric expression");

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
			parse_error("Expected ']'");
		get_next_token();		// primed for next iteration of target-element loop
		}
	else if (token[0] != '\0')
		parse_error("Expected '['");
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
ANT_string_pair terms;
ANT_NEXI_term *answer;

successful_parse = TRUE;
at = string = (unsigned char *)expression;

get_next_token();

if (token.true_strcmp("//") == 0)		// we're a CAS query
	answer = read_CAS();
else
	answer = read_CO(NULL, &terms);

if (token[0] != '\0')
	parse_error("Unexpected end of query");

return successful_parse ? answer : NULL;
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
