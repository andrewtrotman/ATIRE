/*
	NEXI.C
	------
*/
#include "nexi.h"
#include "ctypes.h"
#include "parser.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_NEXI::ISPART()
	------------------
*/
long ANT_NEXI::ispart(char *from, long length, char next)
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
		if (ANT_isdigit(next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_parser::isXMLnamestartchar(*from))
		if (ANT_parser::isXMLnamechar(next))
			return TRUE;
		else if (next == '-')		// hyphenated words
			return TRUE;
		else if (next == '\'')			// apostrophies (don't tell me)
			return TRUE;
		else
			return FALSE;
	else if (ANT_isdigit(*from))
		if (ANT_isdigit(next))
			return TRUE;
		else
			return FALSE;
	else if (*from == '/')
		if (next == '/')
			return TRUE;
		else
			return FALSE;
	else
		return FALSE;
else
	{
	if (*from == '-')
		if (ANT_isdigit(next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_parser::isXMLnamestartchar(*from))
		if (ANT_parser::isXMLnamechar(next))
			return TRUE;
		else
			return FALSE;
	else if (ANT_isalnum(*from))
		if (ANT_isalnum(next))
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
long length;

length = 0;

if (*at != '\0')
	{
	while (ANT_isspace(*at) && *at != '\0')
		at++;
	if (*at != '\0')
		while (*(at + length) != '\0' && ispart(at, length, *(at + length)))
			length++;
	}

token.start = at;
token.string_length = length;
at += length;
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
	else if (token.strcmp("//") == 0)
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
printf("NEXI ERROR, column %ld: %s\n", (long)(at - string), message);
successful_parse = FALSE;
}

/*
	ANT_NEXI::ABOUT()
	-----------------
*/
long ANT_NEXI::about(void)
{
ANT_string_pair path, terms;

get_next_token();
if (token[0] != '(')
	parse_error("Expected '('");
get_next_token();						// prime the read_path method with the first token in the path
read_path(&path);
if (token[0] != ',')
	parse_error("Expected ','");
get_next_token();			// prime read_CO
read_CO(&path, &terms);
if (token[0] != ')')
	parse_error("Expected ')'");

return 0;
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
long ANT_NEXI::numbers(void)
{
ANT_string_pair path, term;

read_path(&path);
read_operator();
term = token;
//read_CO(&path, &terms);

printf("0TAG0:%*.*s:", path.string_length, path.string_length, path.start);
printf("0TERM0:%*.*s:\n",  term.string_length, term.string_length, term.start);

return 0;
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
void ANT_NEXI::read_CO(ANT_string_pair *path, ANT_string_pair *terms)
{
ANT_string_pair current;
long weight, more = TRUE;

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
	else if (!ANT_isalnum(token[0]) && token[0] != '-')		// negative numbers
		more = FALSE;
	if (more)
		{
		printf("TERM:%*.*s:%d:\n",  current.string_length, current.string_length, current.start, weight);
		current = *get_next_token();
		}
	}
while (more);

terms->string_length = token.start - terms->start;

if (path != NULL)
	printf("TAG:%*.*s:", path->string_length, path->string_length, path->start);
printf("TERM:%*.*s:\n",  terms->string_length, terms->string_length, terms->start);
}

/*
	ANT_NEXI::READ_CAS()
	--------------------
*/
long ANT_NEXI::read_CAS(void)
{
ANT_string_pair path;
long parsing_result = FALSE;

do
	{
	read_path(&path);
	if (token[0] == '[')
		{
		do
			{
			get_next_token();

			if (token.strcmp("about") == 0)
				about();
			else if (token[0] == '.')
				numbers();
			else
				parse_error("Expected 'about()' clause or numeric expression");
			get_next_token();
			}
		while (token.strcmp("or") == 0 || token.strcmp("and") == 0 || token.strcmp("OR") == 0 || token.strcmp("AND") == 0);
		if (token[0] != ']')
			parse_error("Expected ']'");
		}
	else if (token[0] != '\0')
		parse_error("Expected '['");
	}
while (token[0] != '\0');

return parsing_result;
}

/*
	ANT_NEXI::PARSE()
	-----------------
*/
void ANT_NEXI::parse(char *expression)
{
ANT_string_pair terms;

successful_parse = TRUE;
at = string = expression;

get_next_token();

if (token.strcmp("//") == 0)		// we're a CAS query
	read_CAS();
else
	read_CO(NULL, &terms);
}

/*
	MAIN()
	------
*/
int main(void)
{
ANT_NEXI nexi_parser;
char buffer[1024];

for(;;)
	{
	printf("]");
	gets(buffer);
	if (strcmp(buffer, ".quit") == 0)
		break;
	nexi_parser.parse(buffer);
	puts("");
	}

return 0;
}

